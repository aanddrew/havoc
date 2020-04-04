#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Server_Sender.h"
#include "../utils/Vector.h"
#include "Server_Pool.h"

//static UDPsocket sockets[16];
static UDPsocket socket;
static int num_clients = 0;
static SDL_Thread* thread;

static SDL_mutex* sockets_mutex;

unsigned short output_port;

void Server_Sender_init(unsigned short port) {
    num_clients = 0;
    sockets_mutex = SDL_CreateMutex();
    output_port = port;
    socket = SDLNet_UDP_Open(output_port);
}

void Server_Sender_deinit() {
    SDL_DestroyMutex(sockets_mutex);
}

static int which_client(IPaddress addr) {
    for(int i = 0; i < num_clients; i++) {
        IPaddress* channel_addr = SDLNet_UDP_GetPeerAddress(socket, i);
        if (addr.host == channel_addr->host)
            return i;
    }
    return -1;
}

void Server_Sender_add_client(IPaddress addr) {
    if (which_client(addr) < 0) {
        printf("Adding client %s\n", SDLNet_ResolveIP(&addr));
        IPaddress new_address;
        SDLNet_ResolveHost(&new_address, SDLNet_ResolveIP(&addr), 21433);
        int channel = SDLNet_UDP_Bind(socket, num_clients, &new_address);
        if(channel==-1) {
            printf("SDLNet_UDP_Bind: %s\n", SDL_GetError());
        }
        num_clients++;
    }
}

static int running = 1;

static int thread_fun(void* arg) {
    pool_t* pool = (pool_t*) arg;

    while(running) {
        SDL_LockMutex(pool->sending_mutex);
        while(pool->sending->num > 0) {
            UDPpacket* pack = Vector_pop(pool->sending);
            for (int j = 0; j < num_clients; j++) {
                IPaddress* j_addr = SDLNet_UDP_GetPeerAddress(socket, j);
                printf("Sending a packet \'%s\' to %s:%d\n", pack->data, SDLNet_ResolveIP(j_addr), j_addr->port);
                pack->address = *j_addr;
                SDLNet_UDP_Send(socket, j, pack);
            }
            SDLNet_FreePacket(pack);
        }
        SDL_UnlockMutex(pool->sending_mutex);
        SDL_LockMutex(pool->running_mutex);
            running = pool->running;
        SDL_UnlockMutex(pool->running_mutex);
    }
    return 0;
}

void Server_Sender_run() {
    thread = SDL_CreateThread(thread_fun, "sender_thread", &shared_pool);
}

void Server_Sender_stop() {
    int value;
    SDL_WaitThread(thread, &value);
    printf("Value is %d\n", value);
}
