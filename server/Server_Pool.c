#include "Server_Pool.h"

#include "../utils/Vector.h"

pool_t shared_pool;

int Pool_get_client_id(IPaddress address)
{
    SDL_LockMutex(shared_pool.clients_mutex);
    int id = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!shared_pool.clients[i].active)
            continue;

        if (address.host == shared_pool.clients[i].address.host
            && address.port == shared_pool.clients[i].address.port)
            id = i;
    }
    SDL_UnlockMutex(shared_pool.clients_mutex);
    return id;
}

void Pool_init()
{
    shared_pool.received = malloc(sizeof(Vector));
    shared_pool.received_swap = malloc(sizeof(Vector));
    shared_pool.sending = malloc(sizeof(Vector));
    Vector_init(shared_pool.received);
    Vector_init(shared_pool.received_swap);
    Vector_init(shared_pool.sending);
    shared_pool.running = 1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        shared_pool.clients[i].active = 0;
    }

    shared_pool.clients_mutex = SDL_CreateMutex();
    shared_pool.received_mutex = SDL_CreateMutex();
    shared_pool.sending_mutex = SDL_CreateMutex();
    shared_pool.running_mutex = SDL_CreateMutex();
}

void Pool_deinit()
{
    Vector_delete(shared_pool.received);
    Vector_delete(shared_pool.received_swap);
    Vector_delete(shared_pool.sending);
    free(shared_pool.received);
    free(shared_pool.received_swap);
    free(shared_pool.sending);

    SDL_DestroyMutex(shared_pool.clients_mutex);
    SDL_DestroyMutex(shared_pool.received_mutex);
    SDL_DestroyMutex(shared_pool.sending_mutex);
    SDL_DestroyMutex(shared_pool.running_mutex);
}
