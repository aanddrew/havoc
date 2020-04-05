#include "Client_Pool.h"

#include "../../utils/Vector.h"

pool_t shared_pool;

Packet* Packet_create(Uint8* data, int len, int sender_id) {
    Packet* pack = malloc(sizeof(Packet));
    pack->data = malloc(len);
    for(int i = 0; i < len; i++) {
        pack->data[i] = data[i];
    }
    pack->len = len;
    pack->sender_id = sender_id;
    return pack;
}

void Packet_destroy(Packet* pack) {
    free(pack->data);
    free(pack);
}

void Pool_init() {
    shared_pool.received = malloc(sizeof(Vector));
    shared_pool.sending = malloc(sizeof(Vector));
    Vector_init(shared_pool.received);
    Vector_init(shared_pool.sending);

    shared_pool.running = 1;

    shared_pool.server_set = SDLNet_AllocSocketSet(1);

    shared_pool.server_mutex = SDL_CreateMutex();
    shared_pool.received_mutex = SDL_CreateMutex();
    shared_pool.sending_mutex = SDL_CreateMutex();
    shared_pool.running_mutex = SDL_CreateMutex();
}

void Pool_deinit() {
    Vector_delete(shared_pool.received);
    Vector_delete(shared_pool.sending);
    free(shared_pool.received);
    free(shared_pool.sending);

    shared_pool.running = 0;

    SDLNet_FreeSocketSet(shared_pool.server_set);

    SDL_DestroyMutex(shared_pool.server_mutex);
    SDL_DestroyMutex(shared_pool.received_mutex);
    SDL_DestroyMutex(shared_pool.sending_mutex);
    SDL_DestroyMutex(shared_pool.running_mutex);
}
