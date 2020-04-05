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
            //send network packets
            Uint8 data[64];
            //write position
            Uint32 my_x = *((int*)&(players[our_id]->pos.x));
            Uint32 my_y = *((int*)&(players[our_id]->pos.y));
            SDLNet_Write32(my_x, data);
            SDLNet_Write32(my_y, data + 4);
            
            //write velocity
            Uint32 my_vel_x = *((int*)&(players[our_id]->vel.x));
            Uint32 my_vel_y = *((int*)&(players[our_id]->vel.y));
            SDLNet_Write32(my_vel_x, data + 8);
            SDLNet_Write32(my_vel_y, data + 12);

            //write look vector
            Uint32 my_look_x = *((int*)&(players[our_id]->look.x));
            Uint32 my_look_y = *((int*)&(players[our_id]->look.y));
            SDLNet_Write32(my_look_x, data + 16);
            SDLNet_Write32(my_look_y, data + 20);

            Packet* pack = Packet_create(data, 24, 0);
            Network_send_packet(pack);

            // Receive Network Packets 

            //printf("queue size: %d\n", Client_Receiver_queuesize());
            int queuesize = Client_Receiver_queue_full_slots();
            #define SIZE_MSG 28
            if (queuesize > SIZE_MSG) {
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

                int num_to_read = queuesize - (queuesize % SIZE_MSG);
                Uint8* msg = malloc(num_to_read);
                //printf("Reading %d bytes\n", num_to_read);
                int read = Client_Receiver_getbytes(msg, num_to_read);
                if (read) {
                    for(int i = 0; i < num_to_read; i += SIZE_MSG) {
                        int id = SDLNet_Read32(msg + i);
                        if (id < 0 || id >= MAX_PLAYERS) {
                            continue;
                        }
                        if (players[id] == NULL) {
                            players[id] = malloc(sizeof(Player));
                            Player_init_wizard(players[id], window->renderer);
                        }

                        if (id != our_id) {
                            Uint32 x = SDLNet_Read32(msg + i + 4);
                            Uint32 y = SDLNet_Read32(msg + i + 8);
                            players[id]->pos.x = *((float*)&x);
                            players[id]->pos.y = *((float*)&y);

                            Uint32 vel_x = SDLNet_Read32(msg + i + 12);
                            Uint32 vel_y = SDLNet_Read32(msg + i + 16);
                            players[id]->vel.x = *((float*)&vel_x);
                            players[id]->vel.y = *((float*)&vel_y);

                            Uint32 look_x = SDLNet_Read32(msg + i + 20);
                            Uint32 look_y = SDLNet_Read32(msg + i + 24);
                            players[id]->look.x = *((float*)&look_x);
                            players[id]->look.y = *((float*)&look_y);

                            printf("%d | %f %f : %f %f\n", 
                                    id,
                                    players[id]->pos.x,
                                    players[id]->pos.y,
                                    players[id]->vel.x,
                                    players[id]->vel.y);
                        }
                    }
                }
                free(msg);

            } //done reading from queue

        }

        /* Update own copy of the game*/

        float dt_float = ((float) dt) / 1000.0f;
        Controller_update(&c, dt_float, &cam);
        Proj_update_all(dt_float);

        Window_clear(window);
		
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if (players[i] != NULL) {
                Player_update(players[i], dt_float);
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
