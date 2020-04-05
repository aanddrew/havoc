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
    SDL_LockMutex(queue_mutex);
        int ret = ByteQueue_getbytes(&queue, output, size);
    SDL_UnlockMutex(queue_mutex);
    return ret;
}

int Client_Receiver_queue_full_slots() {
    SDL_LockMutex(queue_mutex);
        int ret = ByteQueue_full(&queue);
    SDL_UnlockMutex(queue_mutex);
    return ret;
}

Vector* Client_Receiver_get_received() {
    SDL_LockMutex(shared_pool.received_mutex);
        Vector* temp = shared_pool.received;
        shared_pool.received = shared_pool.received_swap;
        shared_pool.received_swap = temp;
    SDL_UnlockMutex(shared_pool.received_mutex);
    return shared_pool.received_swap;
}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    char buffer[PACKET_SIZE];
    while(running) {
        UDPpacket* pack = SDLNet_AllocPacket(PACKET_SIZE);
        //get some bytes from the server
        SDL_LockMutex(shared_pool.server_mutex);
            int numrecv = SDLNet_UDP_Recv(shared_pool.server, pack);
        SDL_UnlockMutex(shared_pool.server_mutex);

        //add them to our queue of received bytes
        if (numrecv) {
            SDL_LockMutex(shared_pool.received_mutex);
                Vector_push(shared_pool.received, pack);
            SDL_UnlockMutex(shared_pool.received_mutex);
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
