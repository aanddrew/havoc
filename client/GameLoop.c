#include "GameLoop.h"

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../gui/Gui.h"

#include "../renderers/GameRenderer.h"
#include "../renderers/MapRenderer.h"

#include "Hud.h"

#include "../game/Game.h"

#include "Controller.h"

#include "network/Client_Pool.h"
#include "network/Client_Receiver.h"
#include "network/Client_Sender.h"
#include "network/Network.h"

#include "../utils/Network_utils.h"

#include "States.h"

enum PAUSEMENU_ITEMS {
    PAUSEMENU_DISCONNECT_BTN,
    PAUSEMENU_NUM_ITEMS,
};

int Game_Loop(Window* window, const char* server_hostname, const char* wish_name)
{
    int ret = EXIT_PROGRAM;

    /* Initialize Network */
    int online = 0;
    Network_init();
    online = Network_connect(server_hostname);
    printf("Joining with name: %s\n", wish_name);
    if (!online) {
        printf("You are offline\n");
        Network_deinit();
    }
    else {
        printf("Successfully connected\n");
    }

    int our_id = 0;
    if (online) {
        our_id = (int)Network_get_our_id();
    }

    /* Pause Menu */
    Menu pause_menu;
    Menu_init(&pause_menu);
    
    int pausemenu_ids[PAUSEMENU_NUM_ITEMS];

    {
        Button disconnect_btn;
        Button_init_text(&disconnect_btn, "Disconnect", 16);
        disconnect_btn.rect.x = -disconnect_btn.rect.w / 2;
        disconnect_btn.rect.y = -disconnect_btn.rect.h / 2;
        disconnect_btn.centerx = 1;
        disconnect_btn.centery = 1;

        pausemenu_ids[PAUSEMENU_DISCONNECT_BTN] = Menu_add_button(&pause_menu, disconnect_btn);
    }

    /* Initialize Game */

    Game_init();
    GameRenderer_init(window->renderer);
    MapRenderer_init(window->renderer);

    Hud_init();

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
    int is_paused = 0;
    while (!done) {
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (is_paused) {
                Menu_pass_event(&pause_menu, window->renderer, &e);
            }
            switch (e.type) {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    is_paused = !is_paused;
                }
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
                if (id != (Uint32)-1 && !Player_get(id)) {
                    Player_connect_with_id("new player", id);
                }
                print_packet(pack);

                //all other events
                switch (message_type) {
                case PLAYER_UPDATE:
                    if (id != (Uint32)our_id) {
                        Network_decipher_player_packet(pack, Player_get(id), 0);
                    } else {
                        Network_decipher_own_player_packet(pack, Player_get(id));
                    }

                    break;
                case PLAYER_DIE:
                    Network_decipher_player_die_packet(pack);
                    break;
                case PLAYER_SPAWN:
                    Network_decipher_player_spawn_packet(pack);
                    break;
                case PLAYER_DISCONNECT:
                    Network_decipher_player_disconnect_packet(pack);
                    break;
                case PROJECTILE_LAUNCH:
                    Network_decipher_projectile_packet(pack, NULL);
                    break;
                case PROJECTILE_DEATH:
                    Network_decipher_projectile_death_packet(pack);
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
        if (is_paused || !our_player->is_alive) {
            Controller_unpress_all(&c);
        }

        if (is_paused) {
            int btn = pause_menu.selected_button;
            if (btn == pausemenu_ids[PAUSEMENU_DISCONNECT_BTN]) {
                done = 1;
                ret = MAIN_MENU;
            }
        }

        Controller_update(&c, &cam);
        Game_update(dt_float);

        Map_collide_player(Game_getmap(), our_player);

        Window_clear(window);
        Map_render(Game_getmap(), window->renderer, &cam);
        GameRenderer_render(window->renderer, &cam);
        Hud_render(window->renderer, our_player);
        if (is_paused) {
            SDL_Rect bg;
            bg.x = 0;
            bg.y = 0;
            SDL_GetRendererOutputSize(window->renderer, &bg.w, &bg.h);

            SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 75);
            SDL_RenderFillRect(window->renderer, &bg);

            Menu_render(&pause_menu, window->renderer);
        }
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
    Hud_deinit();
    GameRenderer_deinit();
    MapRenderer_deinit();
    return ret;
}
