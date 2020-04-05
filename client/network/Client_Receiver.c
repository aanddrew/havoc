#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Receiver.h"
#include "Client_Pool.h"

#include "../../utils/Vector.h"

static SDL_Thread* thread;

#define QUEUE_SIZE 4096
static Uint8* queue;
static int queue_head;
static int queue_tail;

static SDL_mutex* queue_mutex;

void Client_Receiver_init() {
    queue = malloc(QUEUE_SIZE);
    queue_head = 0;
    queue_tail = 0;
    queue_mutex = SDL_CreateMutex();
}

void Client_Receiver_deinit() {
    free(queue);
    SDL_DestroyMutex(queue_mutex);
}

static void queue_insert(Packet* pack) {
    SDL_LockMutex(queue_mutex);
    int offset = 0;
    if (queue_tail < queue_head) {
        offset = QUEUE_SIZE;
    }
    if (queue_tail - queue_head + offset < pack->len) {
        printf("Unable to write, queue full\n");
        return;
    }
    for (int i = 0; i < pack->len; i++) {
        queue[(queue_tail + i) % QUEUE_SIZE] = pack->data[i];
    }
    queue_tail = (queue_tail + pack->len) % QUEUE_SIZE;
    SDL_UnlockMutex(queue_mutex);
}

Uint8* Client_Receiver_getbytes(int size) {
    SDL_LockMutex(queue_mutex);
    int offset = 0;
    if (queue_tail < queue_head) {
        offset = QUEUE_SIZE;
    }
    if (queue_tail - queue_head + QUEUE_SIZE < size) {
        return NULL;
    }

    Uint8* data = malloc(size);
    for (int i = 0; i < size; i++) {
        data[i] = queue[i + queue_head % QUEUE_SIZE];
    }
    SDL_UnlockMutex(queue_mutex);
    queue_head = (queue_head+ size) % QUEUE_SIZE;
    return data;
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
                queue_insert(newpack);
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
