#include <stdio.h>

#include <SDL2/SDL_net.h>

#include "../utils/Vector.h"

#include "Server_Receiver.h"
#include "Server_Sender.h"
#include "Server_Pool.h"

typedef struct {
    IPaddress addr;
} Client;

#define MAX_CLIENTS 16
static IPaddress clients[MAX_CLIENTS];
int num_clients = 0;

int which_client(IPaddress addr) {
    for(int i = 0; i < num_clients; i++) {
        if (addr.port == clients[i].port && addr.host == clients[i].host)
            return i;
    }
    return -1;
}

int add_client(IPaddress addr) {
    if (which_client(addr) < 0) {
        clients[num_clients] = addr;
        num_clients++;
        printf("Adding client %s\n", SDLNet_ResolveIP(&addr));
    }
}

int main() {
    SDL_Init(0);
    SDLNet_Init();

    Pool_init();
    Server_Receiver_init(6969);
    Server_Sender_init(6969);

    Server_Receiver_run();

    Vector* v = malloc(sizeof(Vector));
    Vector_init(v);
    while(1) {
        SDL_LockMutex(shared_pool.received_mutex);
            if (shared_pool.received->num > 0) {
                Vector* temp = v;
                v = shared_pool.received;
                shared_pool.received = temp;
            }
        SDL_UnlockMutex(shared_pool.received_mutex);
        while(v->num > 0) {
            UDPpacket* packet = Vector_pop(v);
            printf("%s\n", packet->data);
            SDLNet_FreePacket(packet);
            add_client(packet->address);
        }
    }

    Server_Receiver_stop();

    SDLNet_Quit();
    return 0;
}
