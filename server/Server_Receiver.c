#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Server_Receiver.h"
#include "Server_Pool.h"

#include "../utils/Vector.h"

static UDPsocket sock;
static SDL_Thread* thread;

void Server_Receiver_init(short port) {
    sock = SDLNet_UDP_Open(port);
}

static int done = 0;

void Clone_packet(UDPpacket* source, UDPpacket* dest) {
    dest->channel = source->channel;
    for(int i = 0; i < source->len; i++) {
        dest->data[i] = source->data[i];
    }
    dest->len = source->len;
    dest->maxlen = source->maxlen;
    dest->status = source->status;
    dest->address = source->address;
}

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    UDPpacket* pack = SDLNet_AllocPacket(1024);
    while(!done) {
        int numrecv = SDLNet_UDP_Recv(sock, pack);
        if (numrecv) {
            UDPpacket* new_pack = SDLNet_AllocPacket(1024);
            Clone_packet(pack, new_pack);
            SDL_LockMutex(pool->received_mutex);
                Vector_push(pool->received, new_pack);
            SDL_UnlockMutex(pool->received_mutex);
        }
    }
    SDLNet_FreePacket(pack);
    return 0;
}

void Server_Receiver_run() {
    thread = SDL_CreateThread(thread_fun, "receiever_thread", &shared_pool);
}

void Server_Receiver_stop() {
    int value;
    SDL_WaitThread(thread, &value);
    printf("Value is %d\n", value);
}
