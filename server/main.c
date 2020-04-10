#include <stdio.h>

#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"

#include "Server_Receiver.h"
#include "Server_Sender.h"
#include "Server_Pool.h"

#include "../game/Projectile.h"
#include "../game/Player.h"
#include "../game/Game.h"
#include "../utils/Network_utils.h"

#define HAVOC_SERVER

static int delta = 10; //10 ms between update times

int main() {
    SDL_Init(0);
    SDLNet_Init();
    Pool_init();

    Game_init();

    Server_Receiver_run(23432);
    Server_Sender_run();

    int last_time = SDL_GetTicks();
    while(1) {
        int current_time = SDL_GetTicks();
        //update loop
        while(current_time > last_time + delta) {
            Vector* received = Server_Receiver_get_received();
            while(received->num > 0) {
                UDPpacket* pack = Vector_pop(received);
                int id = SDLNet_Read32(pack->data);
                int type = SDLNet_Read32(pack->data + 4);
                switch(type) {
                case PLAYER_UPDATE:
                    if (!Player_get(id)) {
                        Player_connect("unknown_player", NULL);
                    }
                    break;
                case CHANGE_NAME:
                    Player_set_name((char*) (pack->data + 8), id);
                    break;
                case PROJECTILE_LAUNCH:
                        //Network_decipher_projectile_packet(pack, NULL);
                    break;
                case GET_NAMES:
                    SDLNet_FreePacket(pack);
                    pack = Network_create_receive_names_packet();
                    break;
                }
                SDL_LockMutex(shared_pool.sending_mutex);
                    Vector_push(shared_pool.sending, pack);
                SDL_UnlockMutex(shared_pool.sending_mutex);
            }

            Proj_update_all(delta);

            last_time += delta;
        }
        //now send other info back to players
    }

    Server_Receiver_stop();
    Server_Sender_stop();

    Pool_deinit();

    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
