#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Server_Sender.h"
#include "../utils/Vector.h"
#include "Server_Pool.h"

static Vector sockets;
static SDL_Thread* thread;

void Server_Sender_init(short port) {
    Vector_init(&sockets);
}

void Server_Sender_deinit() {
    Vector_delete(&sockets);
}

static int done = 0;

static int thread_fun(void* arg) {
    UDPpacket* pack = SDLNet_AllocPacket(1024);
    SDLNet_FreePacket(pack);
    return 0;
}

void Server_Sender_run() {
    thread = SDL_CreateThread(thread_fun, "receiever_thread", &shared_pool);
    SDL_Delay(5000);
    done = 1;
}

void Server_Sender_stop() {
    int value;
    SDL_WaitThread(thread, &value);
    printf("Value is %d\n", value);
}
