#ifndef SERVER_POOL_H
#define SERVER_POOL_H

#include "../utils/Vector.h"
#include "../client/network/ByteQueue.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

//
// This module is responsible for information that is shared 
// by all files in the server
//

#define MAX_CLIENTS 16
typedef struct {
    int active;
    IPaddress address;
} Client;

typedef struct {
    Vector* received;
    Vector* received_swap;
    Vector* sending;
    int running;

    UDPsocket server;
    SDL_mutex* server_mutex;

    Client clients[MAX_CLIENTS];
    SDL_mutex* clients_mutex;

    SDL_mutex* received_mutex;
    SDL_mutex* sending_mutex;
    SDL_mutex* running_mutex;
} pool_t;

extern pool_t shared_pool;

int Pool_get_client_id(IPaddress address);

void Pool_init();
void Pool_deinit();

#endif
