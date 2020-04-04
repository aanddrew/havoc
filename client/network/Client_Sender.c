#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Client_Sender.h"
#include "../../utils/Vector.h"
#include "Client_Pool.h"

//static UDPsocket sockets[16];
static UDPsocket socket;
static SDL_Thread* thread;

unsigned short output_port;

void Client_Sender_init(unsigned short port) {
    output_port = port;
    socket = SDLNet_UDP_Open(output_port);
}

void Client_Sender_deinit() {
    SDLNet_UDP_Close(socket);
    socket = NULL;
}

void Client_Sender_connect(const char* host) {
    printf("Connecting to server: %s\n", host);

    IPaddress new_address;
    SDLNet_ResolveHost(&new_address, host, 23432);

    int channel = SDLNet_UDP_Bind(socket, 0, &new_address);

    if(channel==-1) {
        printf("SDLNet_UDP_Bind: %s\n", SDL_GetError());
    }
    char* connect_msg = "connect";
    UDPpacket* pack = SDLNet_AllocPacket(1024);
    for(int i = 0; i <= (int) strlen(connect_msg); i++) {
        ((char*)pack->data)[i] = connect_msg[i];
    }
    pack->len = strlen(connect_msg) + 1;
    SDL_LockMutex(shared_pool.sending_mutex);
        Vector_push(shared_pool.sending, pack);
    SDL_UnlockMutex(shared_pool.sending_mutex);
}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    IPaddress* send_address = SDLNet_UDP_GetPeerAddress(socket, 0);

    while(running) {
        SDL_LockMutex(pool->sending_mutex);
        while(pool->sending->num > 0) {
            UDPpacket* pack = Vector_pop(pool->sending);

            printf("Sending a packet \'%s\' to %s:%d\n", pack->data, SDLNet_ResolveIP(send_address), send_address->port);

            pack->address = *send_address;
            SDLNet_UDP_Send(socket, 0, pack);

            SDLNet_FreePacket(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);
        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Client_Sender_run() {
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Client_Sender_stop() {
    int value;
    SDL_WaitThread(thread, &value);
}
