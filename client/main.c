#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

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
#include "network/Message.h"

#include "menus/MainMenu.h"
#include "menus/ConnectMenu.h"

#ifdef WIN32
    #define strdup _strdup
#endif

char server_hostname[512];

enum MENU_RET {
    EXIT_PROGRAM,
    MAIN_MENU,
    CONNECT, 
    GAME,
    OPTIONS,
};

int MainMenu_Loop(Window* window);
int ConnectMenu_Loop(Window* window);
int Game_Loop(Window* window);

int main(int argc, char** argv)
{
    Window* window = Window_init();
    int current_state = MAIN_MENU;

    while (current_state != EXIT_PROGRAM) {
        switch (current_state) {
        case MAIN_MENU:
            current_state = MainMenu_Loop(window);
            break;
        case CONNECT:
            current_state = ConnectMenu_Loop(window);
            break;
        case OPTIONS:
            current_state = MainMenu_Loop(window);
            break;
        case GAME:
            current_state = Game_Loop(window);
            break;
        }
    }
}

int ConnectMenu_Loop(Window* window) {
    ConnectMenu_init(window->renderer);

	SDL_Event e;
	int done = 0;
	int ret = EXIT_PROGRAM;
	while (!done) {
		while (SDL_PollEvent(&e)) {
			switch (e.type)
			{
			case SDL_QUIT:
				done = 1;
				ret = EXIT_PROGRAM;
				break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    done = 1;
					ret = MAIN_MENU;
                }
                if (e.key.keysym.sym == SDLK_RETURN) {
                    ConnectMenu_getip(server_hostname, 512);
                    done = 1;
                    ret = GAME;
                }
                break;
            case SDL_MOUSEBUTTONDOWN: {
                int mousex, mousey;
                SDL_GetMouseState(&mousex, &mousey);
                int button_pressed = ConnectMenu_pressed_button(mousex, mousey);
                switch (button_pressed) {
                case CONNECTMENU_JOIN_BUTTON:
                    ConnectMenu_getip(server_hostname, 512);
                    done = 1;
                    ret = GAME;
                    break;
                case CONNECTMENU_BACK_BUTTON:
                    done = 1;
                    ret = MAIN_MENU;
                    break;
                }
            } break;
			}
            ConnectMenu_event(e);
		}
		Window_clear(window);
		ConnectMenu_render(window->renderer);
		Window_present(window);
	}

	ConnectMenu_deinit();
	return ret;
}

int MainMenu_Loop(Window* window) {
	/* Initialize Main Menu */
	MainMenu_init(window->renderer);
	SDL_Event e;
	int done = 0;
    int ret = EXIT_PROGRAM;
	while (!done) {
		while(SDL_PollEvent(&e)) {
            switch (e.type)
            {
				case SDL_MOUSEBUTTONDOWN: {
					int mousex, mousey;
					SDL_GetMouseState(&mousex, &mousey);
					int button = MainMenu_pressed_button(mousex, mousey);
					if (button >= 0) {
						switch (button) {
                        case QUIT_BUTTON:    done = 1; ret = EXIT_PROGRAM; break;
                        case CONNECT_BUTTON: done = 1; ret = CONNECT; break;
                        case OPTIONS_BUTTON: done = 1; ret = OPTIONS; break;
						}
					}
				} break;
				case SDL_WINDOWEVENT:
					switch (e.window.event) {
					case SDL_WINDOWEVENT_RESIZED: {

					} break;
				} break;
				case SDL_QUIT:
					done = 1;
                    ret = EXIT_PROGRAM;
				break;
			}
		}
		Window_clear(window);
		MainMenu_Render(window->renderer);
		Window_present(window);

	}
    MainMenu_deinit();
	return ret;
}

Game_Loop(Window* window) {
    Proj_init_all_sprites(window->renderer);
    /* Initialize Network */
    int online = 0;
    Network_init();
    online = Network_connect(server_hostname);
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
    for (int i = 0; i < MAX_PLAYERS; i++) {
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


    int current_time = SDL_GetTicks();
    int dt = 0;
    SDL_Event e;
    int done = 0;
    while (!done)
    {
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();

        while (SDL_PollEvent(&e))
        {
            switch (e.type)
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
                    Camera_print(&cam);
                } break;
                }
                break;
            case SDL_QUIT:
                done = 1;
                break;
            }
        }

        if (online) {
            //
            //SEND our information to the server
            //

            UDPpacket* pos_pack = Network_create_player_packet(players[our_id]);
            Network_send_packet(pos_pack);

            //
            // Receive Information from the server
            //

            Vector* incoming = Client_Receiver_get_received();
            Vector_reverse(incoming);
            while (incoming->num > 0) {
                UDPpacket* pack = Vector_pop(incoming);
                Uint32 id = SDLNet_Read32(pack->data);
                Uint32 message_type = SDLNet_Read32(pack->data + 4);

                if (players[id] == NULL) {
                    players[id] = malloc(sizeof(Player));
                    Player_init_wizard(players[id], window->renderer);
                }

                switch (message_type) {
                case PLAYER_UPDATE:
                    if (id != our_id)
                        Network_decipher_player_packet(pack, players[id]);
                    break;
                case PROJECTILE_LAUNCH:
                    Network_deciper_projectile_packet(pack, NULL);
                    break;
                }

                SDLNet_FreePacket(pack);
            }
        }

        /* Update own copy of the game*/

        float dt_float = ((float)dt) / 1000.0f;
        Controller_update(&c, dt_float, &cam);
        Proj_update_all(dt_float);

        Window_clear(window);

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (players[i] != NULL) {
                Player_update(players[i], dt_float);
                Dolly_render(players[i]->sprite, window->renderer, &cam);
            }
        }
        Proj_render_all(window->renderer, &cam);

        Window_present(window);
    }

    //
    // Cleanup down here
    //

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i] != NULL && players[i]->sprite != NULL) {
            Dolly_delete(players[i]->sprite);
            free(players[i]->sprite);
        }
    }

    Proj_cleanup_all_sprites();

    if (online) {
        SDL_LockMutex(shared_pool.running_mutex);
        shared_pool.running = 0;
        SDL_UnlockMutex(shared_pool.running_mutex);

        Network_disconnect();
        Network_deinit();
    }
    return 0;
}
