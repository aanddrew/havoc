#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Receiver.h"
#include "Client_Pool.h"
#include "ByteQueue.h"

#include "../../utils/Vector.h"

static SDL_Thread* thread;

#define QUEUE_SIZE 4096
static ByteQueue queue;
static SDL_mutex* queue_mutex;

void Client_Receiver_init() {
    queue_mutex = SDL_CreateMutex();
    ByteQueue_init(&queue, QUEUE_SIZE);
}

void Client_Receiver_deinit() {
    ByteQueue_deinit(&queue);
    SDL_DestroyMutex(queue_mutex);
}

int Client_Receiver_getbytes(Uint8* output, int size) {
    return ByteQueue_getbytes(&queue, output, size);
}

int Client_Receiver_queue_full_slots() {
    return ByteQueue_full(&queue);
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
            SDL_LockMutex(queue_mutex);
                ByteQueue_insert(&queue, buffer, numrecv);
            SDL_UnlockMutex(queue_mutex);
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
