#ifndef CLIENT_POOL_H
#define CLIENT_POOL_H

#include "../../utils/Vector.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

//
// Contains items shared between the sender and receiver
//

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

    TCPsocket server;
    SDLNet_SocketSet server_set;
    SDL_mutex* server_mutex;

    SDL_mutex* received_mutex;
    SDL_mutex* sending_mutex;
    SDL_mutex* running_mutex;
} pool_t;

extern pool_t shared_pool;

void Pool_init();
void Pool_deinit();

#endif
