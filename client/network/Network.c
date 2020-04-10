#include "Network.h"
#include "Client_Pool.h"
#include "Client_Receiver.h"
#include "Client_Sender.h"

#include "../../utils/Network_utils.h"

#include "../../game/Player.h"

#include <stdio.h>
#include <SDL2/SDL.h>

static Uint32 our_id = -1;
static int online = 0;

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
    shared_pool.server_address = addr;

    UDPsocket sock = SDLNet_UDP_Open(-1);
    if (!sock) {
        printf("SDLNet_UDP_Open error: %s\n", SDL_GetError());
    }
    if (SDLNet_UDP_Bind(sock, 0, &addr) < 0) {
        printf("Error binding socket\n");
    }

    UDPpacket* pack = SDLNet_AllocPacket(128);
    SDLNet_Write32(CONNECT_REQUEST, pack->data);
    pack->len = 4;
    pack->address = addr;
    SDLNet_UDP_Send(sock, -1, pack);

    int num_tries = 0;
    const int max_tries = 20;
    while(num_tries < max_tries) {
        int num = SDLNet_UDP_Recv(sock, pack);
        if (num) {
            our_id = SDLNet_Read32(pack->data);
            printf("Server gives us id: %d\n", our_id);
            break;
        }
        else {
            //printf("No response\n");
        }
        SDL_Delay(100);
        num_tries++;
    }
    if (num_tries == max_tries) {
        return 0;
    }
    SDLNet_FreePacket(pack);
    
    shared_pool.server = sock;
    Client_Receiver_run();
    Client_Sender_run();

    online = 1;
    return 1;
}

void Network_disconnect() {
    SDL_LockMutex(shared_pool.running_mutex);
        shared_pool.running = 0;
    SDL_UnlockMutex(shared_pool.running_mutex);

    Client_Sender_stop();
    Client_Receiver_stop();

    //send a disconnect request
    UDPpacket* pack = SDLNet_AllocPacket(4);
    SDLNet_Write32(DISCONNECT_REQUEST, pack->data);
    pack->len = 4;
    SDLNet_UDP_Send(shared_pool.server, -1, pack);

    //close the connection
    SDLNet_UDP_Close(shared_pool.server);
    online = 0;
}

void Network_send_packet(UDPpacket* packet) {
    if (!online) return;

    SDL_LockMutex(shared_pool.sending_mutex);
        Vector_push(shared_pool.sending, packet);
    SDL_UnlockMutex(shared_pool.sending_mutex);
}


Uint32 Network_get_our_id() {
    return our_id;
}

int Network_online() {
    return online;
}
