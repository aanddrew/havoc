#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Server_Sender.h"
#include "../utils/Vector.h"
#include "Server_Pool.h"

static SDL_Thread* thread;

void Server_Sender_init() {
}

void Server_Sender_deinit() {
}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    while(running) {
        SDL_LockMutex(pool->num_clients_mutex);
            int num_clients = pool->num_clients;
        SDL_UnlockMutex(pool->num_clients_mutex);

        SDL_LockMutex(pool->sending_mutex);
        while(pool->sending->num > 0) {
            Packet* pack = Vector_pop(pool->sending);

            for(int i = 0; i < num_clients; i++) {
                SDL_LockMutex(pool->clients[i].mutex);
                    SDLNet_TCP_Send(pool->clients[i].socket, pack->data, pack->len);
                SDL_UnlockMutex(pool->clients[i].mutex);
            }

            Packet_destroy(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);
        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Server_Sender_run() {
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Server_Sender_stop() {
    int value;
    SDL_WaitThread(thread, &value);
    printf("Value is %d\n", value);
}
