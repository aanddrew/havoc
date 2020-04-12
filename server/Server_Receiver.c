#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "../client/network/ByteQueue.h"
#include "Server_Pool.h"
#include "Server_Receiver.h"

#include "../utils/Vector.h"

int Server_Receiver_is_client_active(int c)
{
    SDL_LockMutex(shared_pool.clients_mutex);
    int num_clients = shared_pool.num_clients;
    SDL_UnlockMutex(shared_pool.clients_mutex);
    if (c >= num_clients || c < 0) {
        return 0;
    }
    return 1;
}

Vector* Server_Receiver_get_received()
{
    SDL_LockMutex(shared_pool.received_mutex);
    Vector* temp = shared_pool.received;
    shared_pool.received = shared_pool.received_swap;
    shared_pool.received_swap = temp;
    SDL_UnlockMutex(shared_pool.received_mutex);
    return shared_pool.received_swap;
}

//other stuff
//
void clone_packet(UDPpacket* source, UDPpacket* dest, int offset)
{
    dest->len = source->len + offset;
    for (int i = 0; i < source->len; i++) {
        dest->data[i + offset] = source->data[i];
    }
    dest->address = source->address;
    dest->status = source->status;
    dest->channel = source->channel;
}

static SDL_Thread* listen_thread;

static int running = 1;
#define PACKET_SIZE 64

//This function just listens for new connections on the server's port
static int listen_fun(void* arg)
{
    if (arg != NULL) {
        printf("Warning, non-null argument passed to listen_fun\n");
    }
    printf("Starting listening thread\n");
    UDPpacket* temp_packet = SDLNet_AllocPacket(256);
    while (running) {
        //get the number of clients
        SDL_LockMutex(shared_pool.clients_mutex);
        Uint32 num_clients = shared_pool.num_clients;
        SDL_UnlockMutex(shared_pool.clients_mutex);

        SDL_LockMutex(shared_pool.server_mutex);
        int numrecv = SDLNet_UDP_Recv(shared_pool.server, temp_packet);
        SDL_UnlockMutex(shared_pool.server_mutex);

        if (numrecv) {
            //new client connecting
            int id = Pool_get_client_id(temp_packet->address);
            if (id < 0) {
                printf("new client connected, id: %d\n", num_clients);
                //increment num_clients
                SDL_LockMutex(shared_pool.clients_mutex);
                shared_pool.num_clients++;
                SDL_UnlockMutex(shared_pool.clients_mutex);

                //write their id to a packet
                UDPpacket* id_packet = SDLNet_AllocPacket(4);
                SDLNet_Write32(num_clients, id_packet->data);
                id_packet->len = 4;
                id_packet->address = temp_packet->address;

                //send them their id and bind the server socket at  their id's channel
                SDL_LockMutex(shared_pool.server_mutex);
                SDLNet_UDP_Bind(shared_pool.server, num_clients, &temp_packet->address);
                SDLNet_UDP_Send(shared_pool.server, num_clients, id_packet);
                SDL_UnlockMutex(shared_pool.server_mutex);

                SDLNet_FreePacket(id_packet);

                //add their ip address to the clients array
                SDL_LockMutex(shared_pool.clients_mutex);
                shared_pool.clients[num_clients].address = temp_packet->address;
                shared_pool.clients[num_clients].id = num_clients;
                SDL_UnlockMutex(shared_pool.clients_mutex);
            }
            //already connected client is sending a message
            else {
                SDL_LockMutex(shared_pool.received_mutex);
                //prepend the id of the sender to the message
                UDPpacket* cloned_pack = SDLNet_AllocPacket(256);
                SDLNet_Write32((Uint32)id, cloned_pack->data);

                //now just copy the original message
                clone_packet(temp_packet, cloned_pack, 4);
                Vector_push(shared_pool.received, cloned_pack);
                SDL_UnlockMutex(shared_pool.received_mutex);
            }
        }
    }

    printf("Stopping listening thread\n");
    return 0;
}

void Server_Receiver_run(short port)
{
    shared_pool.server = SDLNet_UDP_Open(port);
    //starts the listen thread which accepts clients
    listen_thread = SDL_CreateThread(listen_fun, "listener_thread", NULL);
}

void Server_Receiver_stop()
{
    int value;
    SDL_WaitThread(listen_thread, &value);
    printf("Value is %d\n", value);
}
