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
    Server_Receiver_init(23432);
    Server_Sender_init();

    Server_Receiver_run();
    Server_Sender_run();

    Vector* v = malloc(sizeof(Vector));
    Vector_init(v);
    while(1) {
        int queuesize = Server_Receiver_queue_full_slots();
        if (queuesize > 28) {
            int num_to_read = queuesize - (queuesize % 28);
            Uint8* buffer = malloc(num_to_read);
            Server_Receiver_getbytes(buffer, num_to_read);

            for(int i = 0; i < num_to_read; i+= 28) {
                Uint32 id = SDLNet_Read32(buffer);
                if (id >= MAX_CLIENTS) continue;
                printf("Packet came from id %d\n", id);
                Packet* outgoing = Packet_create(buffer + i, 28, id);
                SDL_LockMutex(shared_pool.sending_mutex);
                    Vector_push(shared_pool.sending, outgoing);
                SDL_UnlockMutex(shared_pool.sending_mutex);
            }
        }
    }

    Server_Receiver_stop();
    Server_Sender_stop();

    Server_Receiver_deinit();
    Server_Sender_deinit();

    SDLNet_Quit();
    return 0;
}
