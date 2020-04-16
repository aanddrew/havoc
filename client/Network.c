#include "Network.h"

#include "../utils/Network_utils.h"
#include "../game/Player.h"

#include <enet/enet.h>

#include <SDL2/SDL.h>
#include <stdio.h>

//data for enet, 
//ONLY ACESS THESE INSIDE OF RECEIVER_FUN OR THE PROGRAM WILL BREAK
static ENetHost * client;
static ENetAddress address;
static ENetPeer* server;

//thread that the receiver fun runs on
SDL_Thread* receiver_thread;
static int running = 1;
SDL_mutex* running_mutex;

//data for game
static Uint32 our_id = -1;
static int online = 0;
SDL_mutex* online_mutex;
static int error = 0;
SDL_mutex* error_mutex;

//packets received
Vector* received;
Vector* received_swap;
SDL_mutex* received_mutex;

Vector sending;
SDL_mutex* sending_mutex;

static void set_online(int new_online) {
    SDL_LockMutex(online_mutex);
    online = new_online;
    SDL_UnlockMutex(online_mutex);
}
static void set_error(int new_error) {
    SDL_LockMutex(error_mutex);
    error = new_error;
    SDL_UnlockMutex(error_mutex);
}
static void set_running(int new_running) {
    SDL_LockMutex(running_mutex);
    running = new_running;
    SDL_UnlockMutex(running_mutex);
}

void Network_init()
{
    if (enet_initialize () != 0)
    {
        printf ("An error occurred while initializing ENet.\n");
        return;
    }

    received = malloc(sizeof(Vector));
    received_swap = malloc(sizeof(Vector));
    Vector_init(received);
    Vector_init(received_swap);
    received_mutex = SDL_CreateMutex();

    Vector_init(&sending);
    sending_mutex = SDL_CreateMutex();

    online_mutex = SDL_CreateMutex();
    error_mutex = SDL_CreateMutex();
    running_mutex = SDL_CreateMutex();

    set_online(0);

    our_id = -1;
}

void Network_deinit()
{
    Vector_delete(received);
    Vector_delete(received_swap);
    free(received);
    free(received_swap);

    SDL_DestroyMutex(received_mutex);
    received_mutex = NULL;

    Vector_delete(&sending);
    SDL_DestroyMutex(sending_mutex);
    sending_mutex = NULL;

    SDL_DestroyMutex(online_mutex);
    online_mutex = NULL;
    SDL_DestroyMutex(error_mutex);
    error_mutex = NULL;
    SDL_DestroyMutex(running_mutex);
    running_mutex = NULL;

    enet_deinitialize();
    set_online(0);
    our_id = -1;
}

int receiver_fun(void* hostname) 
{
    if (!hostname) {
        printf("Error, need ip for connection\n");
    }

    client = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
    if (client == NULL)
    {
        printf ("An error occurred while trying to create an ENet client host.\n");
        set_online(0);
        set_error(1);
        return 0;
    }

    enet_address_set_host(&address, hostname);
    address.port = 23432;

    server = enet_host_connect(client, &address, 2, 0);
    if (!server) {
        printf("Error creating ENet client!\n");
        set_online(0);
        set_error(1);
        return 0;
    }

    {
        //send message twice to establish connection, 
        //packet must be created twice because enet takes ownership of packets
        int connect = CONNECTION;

        ENetPacket *enet_packet = enet_packet_create(&connect, 4, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, enet_packet);

        enet_packet = enet_packet_create(&connect, 4, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, enet_packet);
    }


    ENetEvent event;
    while (running) {
        if (enet_host_service(client, &event, 0)) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("We are connected to the server!\n");
                    break;
                case ENET_EVENT_TYPE_RECEIVE: {
                    printf("Message from server : %s\n", event.packet->data);
                    if (event.packet->dataLength < 4) break;

                    int type = *((int*) event.packet->data);
                    switch(type) {
                    case GIVE_ID:
                        if (event.packet->dataLength == 8) {
                            our_id = *((int*) event.packet->data + 4);
                            printf("Got id %d\n", our_id);
                            set_online(1);
                        }
                        break;
                    default:

                        break;
                    }
                    enet_packet_destroy(event.packet);
                } break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("Lost connection to server\n");
                    set_online(0);
                    break;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }

    return 0;
}

void Network_connect(const char* hostname)
{
    char* hostname_arg = strdup(hostname);
    receiver_thread = SDL_CreateThread(receiver_fun, "receiver_thread", hostname_arg);
}

void Network_disconnect()
{
    set_running(0);
    int status;
    SDL_WaitThread(receiver_thread, &status);
    printf("receiver_thread exited with status %d\n", status);
    set_online(0);
    set_error(0);
}

Vector* Network_get_received() {
    SDL_LockMutex(received_mutex);

    Vector*temp = received;
    received = received_swap;
    received_swap = temp;

    SDL_UnlockMutex(received_mutex);
    return received_swap;
}

void Network_send_packet(ENetPacket* packet)
{
    if (!online)
        return;

    SDL_LockMutex(sending_mutex);
    Vector_push(&sending, packet);
    SDL_UnlockMutex(sending_mutex);
}

Uint32 Network_get_our_id()
{
    return our_id;
}

int Network_online()
{
    return online;
}

int Network_error()
{
    return error;
}
