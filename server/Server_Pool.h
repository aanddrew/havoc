#ifndef SERVER_POOL_H
#define SERVER_POOL_H

#include "../utils/Vector.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define MAX_CLIENTS 16
typedef struct {
    TCPsocket socket;
    SDL_mutex* mutex;
    int id;
} Client;

#define PACKET_SIZE 64
typedef struct {
    Uint8* data;
    int len;
    int sender_id;
} Packet;

Packet* Packet_create(Uint8* data, int len, int sender_id);
void Packet_destroy(Packet* pack);

typedef struct {
    Vector* received;
    Vector* sending;
    int running;

    Client clients[MAX_CLIENTS];
    int num_clients;
    SDL_mutex* num_clients_mutex;

    SDL_mutex* received_mutex;
    SDL_mutex* sending_mutex;
    SDL_mutex* running_mutex;
} pool_t;

extern pool_t shared_pool;

void Pool_init();
void Pool_deinit();

#endif
