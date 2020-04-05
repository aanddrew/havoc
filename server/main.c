#include <stdio.h>

#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"

#include "Server_Receiver.h"
#include "Server_Sender.h"
#include "Server_Pool.h"

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
        SDL_LockMutex(shared_pool.received_mutex);
            if (shared_pool.received->num > 0) {
                Vector* temp = v;
                v = shared_pool.received;
                shared_pool.received = temp;
            }
        SDL_UnlockMutex(shared_pool.received_mutex);
        while(v->num > 0) {
            Packet* message = Vector_pop(v);
            Uint8 buffer[64];
            SDLNet_Write32(message->sender_id, buffer);
            for(int i = 0; i < message->len; i++) {
                buffer[i + 4] = message->data[i];
            }
            Packet_destroy(message);

            Packet* outgoing = Packet_create(buffer, 28, message->sender_id);

            SDL_LockMutex(shared_pool.sending_mutex);
                Vector_push(shared_pool.sending, outgoing);
            SDL_UnlockMutex(shared_pool.sending_mutex);
        }
        /*
        char* msg = "hello, client";
        SDL_LockMutex(shared_pool.sending_mutex);
            Packet* pack = Packet_create((Uint8*) msg, strlen(msg) + 1, 0);
            Vector_push(shared_pool.sending, pack);
        SDL_UnlockMutex(shared_pool.sending_mutex);
        SDL_Delay(100);
        */
    }

    Server_Receiver_stop();
    Server_Sender_stop();

    Server_Receiver_deinit();
    Server_Sender_deinit();

    SDLNet_Quit();
    return 0;
}
