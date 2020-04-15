#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../../utils/Vector.h"
#include "Client_Pool.h"
#include "Client_Sender.h"

static SDL_Thread* thread;

void Client_Sender_init()
{
}

void Client_Sender_deinit()
{
}

static int running = 1;

//looks through packets in the sending vector and sends them to the server
static int thread_fun(void* arg)
{
    pool_t* pool = (pool_t*)arg;

    while (running) {
        SDL_LockMutex(pool->running_mutex);
        running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);

        SDL_LockMutex(pool->sending_mutex);
        while (pool->sending->num > 0) {
            UDPpacket* pack = Vector_pop(pool->sending);
            pack->address = shared_pool.server_address;
            SDL_LockMutex(pool->server_mutex);
            SDLNet_UDP_Send(pool->server, -1, pack);
            SDL_UnlockMutex(pool->server_mutex);
            SDLNet_FreePacket(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);
    }
    printf("Client_Sender stopped\n");
    return 0;
}

void Client_Sender_run()
{
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Client_Sender_stop()
{
    int value;
    SDL_WaitThread(thread, &value);
}
