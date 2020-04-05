#include <SDL2/SDL.h>
#include <string.h>

#include "graphics/Window.h"
#include "graphics/Dolly.h"
#include "graphics/Camera.h"

#include "engine/Player.h"
#include "engine/Controller.h"
#include "engine/Projectile.h"

#include "network/Client_Pool.h"
#include "network/Client_Sender.h"
#include "network/Client_Receiver.h"
#include "network/Network.h"

#ifdef WIN32
    #define strdup _strdup
#endif

int main(int argc, char** argv)
{
    Window* window = Window_init();
    Proj_init_all_sprites(window->renderer);

    /* Initialize Network */

    int online = 0;
    Network_init();
    online = Network_connect("198.58.109.228");
    if (!online) {
        printf("You are offline\n");
        Network_deinit();
    }

    int our_id = 0;
    if (online) {
        our_id = (int)Network_get_our_id();
    } 
 
    /* Initialize Game */

    #define MAX_PLAYERS 16
    Player* players[MAX_PLAYERS];
    for(int i = 0; i < MAX_PLAYERS; i++) {
        players[i] = NULL;
    }

    players[our_id] = malloc(sizeof(Player));
    Player_init_wizard(players[our_id], window->renderer);

    Camera cam;
    Camera_init(&cam);

    {
        SDL_Rect viewport;
        SDL_RenderGetViewport(window->renderer, &viewport);

        Camera_set_size(&cam, viewport.w, viewport.h);
        Camera_set_center(&cam, players[our_id]->pos.x, players[our_id]->pos.y);
    }

    Controller c;
    Controller_init(&c);
    c.player = players[our_id];
    c.cam = &cam;

    SDL_Delay(500);

    Vector* incoming_packets = malloc(sizeof(Vector));
    Vector_init(incoming_packets);

	SDL_Event e;
	int done = 0;
    int current_time = SDL_GetTicks();
    int dt = 0;
	while(!done)
	{
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();

		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
                case SDL_KEYDOWN:
                    Controller_keydown(&c, e.key.keysym.sym);
                break;
                case SDL_KEYUP:
                    Controller_keyup(&c, e.key.keysym.sym);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    Controller_mousebuttondown(&c, e.button);
                break;
                case SDL_MOUSEWHEEL: 
                    Controller_mousewheel(&c, e.wheel);
                break;
                case SDL_WINDOWEVENT:
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_RESIZED: {
                            SDL_Rect viewport;
                            SDL_RenderGetViewport(window->renderer, &viewport);

                            Camera_set_size(&cam, viewport.w, viewport.h);
                        } break;
                    }
                break;
				case SDL_QUIT:
					done = 1;
				break;
			}
		}
        if (online) {
            /* Send Network Packets */
            Uint8 data[64];
            Uint32 my_x = *((int*)&(players[our_id]->pos.x));
            Uint32 my_y = *((int*)&(players[our_id]->pos.y));

            SDLNet_Write32(my_x, data);
            SDLNet_Write32(my_y, data + 4);

            Packet* pack = Packet_create(data, 8, 0);
            Network_send_packet(pack);

            /* Receive Network Packets */

            SDL_LockMutex(shared_pool.received_mutex);
            if (shared_pool.received->num > 0) {
                Vector* temp = incoming_packets;
                incoming_packets = shared_pool.received;
                shared_pool.received = temp;
            }
            SDL_UnlockMutex(shared_pool.received_mutex);
            while (incoming_packets->num > 0) {
                Packet* message = Vector_pop(incoming_packets);
                Packet_destroy(message);
            }

            Uint8* next_12 = Client_Receiver_getbytes(12);

            int id = SDLNet_Read32(next_12);
            if (players[id] == NULL) {
                players[id] = malloc(sizeof(Player));
                Player_init_wizard(players[id], window->renderer);
            }

            if (id != our_id) {
                Uint32 x = SDLNet_Read32(next_12 + 4);
                Uint32 y = SDLNet_Read32(next_12 + 8);
                players[id]->pos.x = *((float*)&x);
                players[id]->pos.y = *((float*)&y);
            }
        }

        /* Update own copy of the game*/

        float dt_float = ((float) dt) / 1000.0f;
        Controller_update(&c, dt_float, &cam);
        Proj_update_all(dt_float);

        Window_clear(window);
		
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if (players[i] != NULL) {
                Player_update_sprite(players[i]);
                Dolly_render(players[i]->sprite, window->renderer, &cam);
            }
        }
        Proj_render_all(window->renderer, &cam);

        Window_present(window);
	}

    for(int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] != NULL && players[i]->sprite != NULL) {
            Dolly_delete(players[i]->sprite);
            free(players[i]->sprite);
        }
    }
    Proj_cleanup_all_sprites();
    Window_delete(window);

    if (online) {
        SDL_LockMutex(shared_pool.running_mutex);
            shared_pool.running = 0;
        SDL_UnlockMutex(shared_pool.running_mutex);

        Network_disconnect();
        Network_deinit();
    }
	return 0;
}
