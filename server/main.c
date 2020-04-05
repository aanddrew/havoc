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
    Uint8* buffer = malloc(4096);
    while(1) {
        for(int c = 0; c < MAX_CLIENTS; c++) {
            int active = Server_Receiver_is_client_active(c);
            if (!active) continue;

            int queuesize = Server_Receiver_queue_full_slots(c);
            if (queuesize > 28) {
                int num_to_read = queuesize - (queuesize % 28);
                Server_Receiver_getbytes(c, buffer, num_to_read);

                for(int i = 0; i < num_to_read; i+= 28) {
                    Uint32 id = SDLNet_Read32(buffer);
                    if (id >= MAX_CLIENTS) continue;
                    printf("Packet came from id %d, c = %d\n", id, c);
                    if ((Uint32) c != id) continue;
                    Packet* outgoing = Packet_create(buffer + i, 28, id);
                    SDL_LockMutex(shared_pool.sending_mutex);
                        Vector_push(shared_pool.sending, outgoing);
                    SDL_UnlockMutex(shared_pool.sending_mutex);
                }
            }
        }
        SDL_Delay(10);
    }
    free(buffer);

    Server_Receiver_stop();
    Server_Sender_stop();

    Server_Receiver_deinit();
    Server_Sender_deinit();

    SDLNet_Quit();
    return 0;
}
