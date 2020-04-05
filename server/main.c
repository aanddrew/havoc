#include <stdio.h>

#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"

#include "Server_Receiver.h"
#include "Server_Sender.h"
#include "Server_Pool.h"

/*
 * The problem is that we are writing messages to a single buffer from each
 * client.
*/

int main() {
    SDL_Init(0);
    SDLNet_Init();

    Pool_init();

    Server_Receiver_run(23432);
    Server_Sender_run();

    while(1) {
        Vector* received = Server_Receiver_get_received();
        while(received->num > 0 ) {
            UDPpacket* pack = Vector_pop(received);
            SDL_LockMutex(shared_pool.sending_mutex);
                Vector_push(shared_pool.sending, pack);
            SDL_UnlockMutex(shared_pool.sending_mutex);
        }
        SDL_Delay(10);
    }

    Server_Receiver_stop();
    Server_Sender_stop();

    Pool_deinit();

    SDLNet_Quit();
    return 0;
}
