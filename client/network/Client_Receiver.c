#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Receiver.h"
#include "Client_Pool.h"

#include "../../utils/Vector.h"

static UDPsocket sock;
static SDL_Thread* thread;

void Client_Receiver_init(short port) {
    sock = SDLNet_UDP_Open(port);
}

void Client_Receiver_deinit() {
    SDLNet_UDP_Close(sock);
    sock = NULL;
}

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

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    UDPpacket* pack = SDLNet_AllocPacket(1024);
    while(running) {
        int numrecv = SDLNet_UDP_Recv(sock, pack);
        if (numrecv) {
            UDPpacket* new_pack = SDLNet_AllocPacket(1024);
            Clone_packet(pack, new_pack);
            SDL_LockMutex(pool->received_mutex);
                printf("%s says: %s\n", SDLNet_ResolveIP(&pack->address), pack->data);
                Vector_push(pool->received, new_pack);
            SDL_UnlockMutex(pool->received_mutex);
        }
        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    SDLNet_FreePacket(pack);
    return 0;
}

void Client_Receiver_run() {
    thread = SDL_CreateThread(thread_fun, "receiver_thread", &shared_pool);
}

void Client_Receiver_stop() {
    int value;
    SDL_WaitThread(thread, &value);
}
