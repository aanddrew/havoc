#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../gui/Window.h"
#include "../gui/Dolly.h"
#include "../gui/Camera.h"
#include "../gui/Fonts.h"
#include "../renderers/MapRenderer.h"

#include "../renderers/GameRenderer.h"

#include "../game/Player.h"
#include "../game/Projectile.h"
#include "../game/Game.h"
#include "../game/Map.h"
#include "Controller.h"

#include "network/Client_Pool.h"
#include "network/Client_Sender.h"
#include "network/Client_Receiver.h"
#include "network/Network.h"

#include "../utils/Network_utils.h"

#include "menus/MainMenu.h"
#include "menus/ConnectMenu.h"

#ifdef WIN32
    #define strdup _strdup
#endif

char server_hostname[512];
char wish_name[64];

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
    if (argc != 1) {
        printf("Error, not supposed to take arguments\n");
        for(int i = 1; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    }
    Window* window = Window_init();
    Fonts_init(window->renderer);
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

    Fonts_deinit();
    Window_delete(window);
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
                int button_pressed = ConnectMenu_pressed_button(window->renderer, mousex, mousey);
                switch (button_pressed) {
                case CONNECTMENU_JOIN_BUTTON:
                    ConnectMenu_getip(server_hostname, 512);
                    ConnectMenu_getname(wish_name, 64);
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
					int button = MainMenu_pressed_button(window->renderer, mousex, mousey);
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

int Game_Loop(Window* window) {
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

    Game_init();
    GameRenderer_init(window->renderer);
    MapRenderer_init(window->renderer);

    Map map;
    Map_init(&map, "editor/maps/test_map.hm");

    Player* our_player = Player_connect_with_id("default", our_id);

    Camera cam;
    Camera_init(&cam, window->renderer);

    Controller c;
    Controller_init(&c);
    c.player = our_player;
    c.cam = &cam;

    int current_time = SDL_GetTicks();
    int last_name_request_time = SDL_GetTicks();
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

            UDPpacket* pos_pack = Network_create_player_packet(our_player);
            Network_send_packet(pos_pack);

            if (strcmp(our_player->name, wish_name)) {
                UDPpacket* namepack = Network_create_change_name_packet(wish_name);
                Network_send_packet(namepack);
            }
            
            if (current_time - last_name_request_time >= 1000) {
                //printf("REQUESTING NAMES\n");
                UDPpacket* namerequestpack = Network_create_get_names_packet();
                Network_send_packet(namerequestpack);
            }

            //
            // Receive Information from the server
            //

            Vector* incoming = Client_Receiver_get_received();
            Vector_reverse(incoming);
            while (incoming->num > 0) {
                UDPpacket* pack = Vector_pop(incoming);
                Uint32 id = SDLNet_Read32(pack->data);
                Uint32 message_type = SDLNet_Read32(pack->data + 4);

                //add new player to the game
                if (id != (Uint32) -1 && !Player_get(id)) {
                    Player_connect_with_id("new player", id);
                }
                
                //all other events
                switch (message_type) {
                case PLAYER_UPDATE:
                    if (id != (Uint32) our_id)
                        Network_decipher_player_packet(pack, Player_get(id));
                    break;
                case PROJECTILE_LAUNCH:
                    Network_decipher_projectile_packet(pack, NULL);
                    break;
                case CHANGE_NAME:
                    Network_decipher_change_name_packet(pack);
                    break;
                case RECEIVE_NAMES:
                    Network_decipher_receive_names_packet(pack);
                    last_name_request_time = current_time;
                    break;
                }

                SDLNet_FreePacket(pack);
            }
        }

        /* Update own copy of the game*/

        float dt_float = ((float)dt) / 1000.0f;
        Controller_update(&c, &cam);
        Game_update(dt_float);
        
        Map_collide_player(&map, our_player);

        Window_clear(window);
        Map_render(&map, window->renderer, &cam);
        GameRenderer_render(window->renderer, &cam);
        Window_present(window);
    }

    //
    // Cleanup down here
    //
    
    Proj_cleanup_all_sprites();

    if (online) {
        SDL_LockMutex(shared_pool.running_mutex);
        shared_pool.running = 0;
        SDL_UnlockMutex(shared_pool.running_mutex);

        Network_disconnect();
        Network_deinit();
    }
    GameRenderer_deinit();
    MapRenderer_deinit();
    return 0;
}
