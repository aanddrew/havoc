#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Receiver.h"
#include "Client_Pool.h"

#include "../../utils/Vector.h"

static SDL_Thread* thread;

void Client_Receiver_init() {

}

void Client_Receiver_deinit() {

}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    char buffer[PACKET_SIZE];
    while(running) {
        int numrecv = 0;
        SDL_LockMutex(shared_pool.server_mutex);
            if (SDLNet_CheckSockets(shared_pool.server_set, 0) > 0) {
                if (SDLNet_SocketReady(shared_pool.server)) {
                    numrecv = SDLNet_TCP_Recv(shared_pool.server, buffer, PACKET_SIZE);
                }
            }
        SDL_UnlockMutex(shared_pool.server_mutex);

        if (numrecv) {
            Packet* newpack = Packet_create(buffer, numrecv, 0);
            SDL_LockMutex(pool->received_mutex);
                //printf("Server says: %s\n", buffer);
                Vector_push(pool->received, newpack);
            SDL_UnlockMutex(pool->received_mutex);
        }

        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Client_Receiver_run() {
    thread = SDL_CreateThread(thread_fun, "receiver_thread", &shared_pool);
}

void Client_Receiver_stop() {
    int value;
    SDL_WaitThread(thread, &value);
}
