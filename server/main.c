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

void add_client(IPaddress addr) {
    if (which_client(addr) < 0) {
        clients[num_clients] = addr;
        num_clients++;
    }
}

int main() {
    SDL_Init(0);
    SDLNet_Init();

    Pool_init();
    Server_Receiver_init(23432);
    Server_Sender_init(23433);

    Server_Receiver_run();
    Server_Sender_run();

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
            
            add_client(packet->address);
            Server_Sender_add_client(packet->address);

            SDL_LockMutex(shared_pool.sending_mutex);
                UDPpacket* sent = SDLNet_AllocPacket(1024);
                char* msg = "Hello, new client";
                for(int i = 0; i < strlen(msg) + 1; i++) {
                    ((char*)sent->data)[i] = msg[i];
                }
                sent->len = strlen(msg) + 1;
                sent->channel = -1;
                sent->status = sent->len;
                Vector_push(shared_pool.sending, sent);
            SDL_UnlockMutex(shared_pool.sending_mutex);
            SDLNet_FreePacket(packet);
        }
    }

    Server_Receiver_stop();
    Server_Sender_stop();

    SDLNet_Quit();
    return 0;
}
