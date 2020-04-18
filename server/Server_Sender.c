#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"
#include "Server_Pool.h"
#include "Server_Sender.h"

static SDL_Thread* thread;

static int running = 1;

static int thread_fun(void* arg)
{
    pool_t* pool = (pool_t*)arg;

    while (running) {

        SDL_LockMutex(pool->sending_mutex);
        while (pool->sending->num > 0) {
            UDPpacket* pack = Vector_pop(pool->sending);

            SDL_LockMutex(pool->clients_mutex);
            SDL_LockMutex(pool->server_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!pool->clients[i].active) continue;

                pack->address = shared_pool.clients[i].address;
                SDLNet_UDP_Send(pool->server, i, pack);
            }
            SDL_UnlockMutex(pool->server_mutex);
            SDL_UnlockMutex(pool->clients_mutex);

            SDLNet_FreePacket(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);
        SDL_LockMutex(pool->running_mutex);
        running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Server_Sender_run()
{
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Server_Sender_stop()
{
    int value;
    SDL_WaitThread(thread, &value);
    printf("Value is %d\n", value);
}
