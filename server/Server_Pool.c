#include "Server_Pool.h"

#include "../utils/Vector.h"

pool_t shared_pool;

void Pool_init() {
    shared_pool.received = malloc(sizeof(Vector));
    shared_pool.sending = malloc(sizeof(Vector));
    Vector_init(shared_pool.received);
    Vector_init(shared_pool.sending);
    shared_pool.running = 1;

    shared_pool.received_mutex = SDL_CreateMutex();
    shared_pool.sending_mutex = SDL_CreateMutex();
    shared_pool.running_mutex = SDL_CreateMutex();
}

void Pool_deinit() {
    SDL_DestroyMutex(shared_pool.received_mutex);
    SDL_DestroyMutex(shared_pool.sending_mutex);
    SDL_DestroyMutex(shared_pool.running_mutex);
}
