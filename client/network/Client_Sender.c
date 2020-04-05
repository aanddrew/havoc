#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Sender.h"
#include "../../utils/Vector.h"
#include "Client_Pool.h"

static SDL_Thread* thread;

void Client_Sender_init() {

}

void Client_Sender_deinit() {
}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    while(running) {
        SDL_LockMutex(pool->sending_mutex);
        while(pool->sending->num > 0) {
            Packet* pack = Vector_pop(pool->sending);
            SDL_LockMutex(pool->server_mutex);
                SDLNet_TCP_Send(pool->server, pack->data, pack->len);
            SDL_UnlockMutex(pool->server_mutex);
            Packet_destroy(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);

        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Client_Sender_run() {
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Client_Sender_stop() {
    int value;
    SDL_WaitThread(thread, &value);
}
