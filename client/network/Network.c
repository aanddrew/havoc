#include "Network.h"
#include "Client_Pool.h"
#include "Client_Receiver.h"
#include "Client_Sender.h"

#include <SDL2/SDL.h>

static Uint32 our_id = -1;

void Network_init() {
    SDLNet_Init();
    Pool_init();
    Client_Receiver_init();
    Client_Sender_init();
}

void Network_deinit() {
    Pool_deinit();
    Client_Receiver_deinit();
    Client_Sender_deinit();
    SDLNet_Quit();
}

int Network_connect(const char* hostname) {
    printf("Requesting to connect to server: %s\n", hostname);

    IPaddress addr;
    if (SDLNet_ResolveHost(&addr, hostname, 23432) < 0) {
        printf("Error resolving host\n");
        return 0;
    }

    TCPsocket sock = SDLNet_TCP_Open(&addr);
    if (!sock) {
        printf("Error connecting to host: %s\n", SDL_GetError());
        return 0;
    }

    char temp[64];
    int done = 0;
    while(!done) {
        int num = SDLNet_TCP_Recv(sock, temp, 64);
        our_id = SDLNet_Read32(temp);
        if (num) {
            printf("Server gives us id: %d\n", our_id);
            done = 1;
        }
    }
    
    shared_pool.server = sock;
    SDLNet_TCP_AddSocket(shared_pool.server_set, shared_pool.server);
    Client_Receiver_run();
    Client_Sender_run();

    return 1;
}

void Network_disconnect() {
    SDL_LockMutex(shared_pool.running_mutex);
        shared_pool.running = 0;
    SDL_UnlockMutex(shared_pool.running_mutex);

    Client_Sender_stop();
    SDLNet_TCP_Close(shared_pool.server);
    Client_Receiver_stop();
}

void Network_send_packet(Packet* packet) {
    SDL_LockMutex(shared_pool.sending_mutex);
        Vector_push(shared_pool.sending, packet);
    SDL_UnlockMutex(shared_pool.sending_mutex);
}

Uint32 Network_get_our_id() {
    return our_id;
}
