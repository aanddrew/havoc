#include <stdio.h>

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>

#include "Server_Receiver.h"
#include "Server_Pool.h"
#include "../client/network/ByteQueue.h"

#include "../utils/Vector.h"

//queue stuff

#define QUEUE_SIZE 4096
static ByteQueue queue;
static SDL_mutex* queue_mutex;

int Server_Receiver_is_client_active(int c) {
    SDL_LockMutex(shared_pool.num_clients_mutex);
        int num_clients = shared_pool.num_clients;
    SDL_UnlockMutex(shared_pool.num_clients_mutex);
    if (c >= num_clients || c < 0) {
        return 0;
    }
    return 1;
}

int Server_Receiver_getbytes(int client_id, Uint8* output, int size) {
    Client* this_client = &shared_pool.clients[client_id];
    SDL_LockMutex(this_client->queue_mutex);
        int ret = ByteQueue_getbytes(this_client->queue, output, size);
    SDL_UnlockMutex(this_client->queue_mutex);
    return ret;
}

int Server_Receiver_queue_full_slots(int client_id) {
    Client* this_client = &shared_pool.clients[client_id];
    SDL_LockMutex(this_client->queue_mutex);
        int ret = ByteQueue_full(this_client->queue);
    SDL_UnlockMutex(this_client->queue_mutex);
    return ret;
}

//other stuff

static IPaddress server_ip;

static TCPsocket server;
SDL_mutex* server_mutex;

static SDL_Thread* client_threads[MAX_CLIENTS];
static char* client_thread_names[MAX_CLIENTS];

static SDL_Thread* listen_thread;

void Server_Receiver_init(short port) {
    SDLNet_ResolveHost(&server_ip, NULL, port);

    queue_mutex = SDL_CreateMutex();
    ByteQueue_init(&queue, QUEUE_SIZE);

    server = SDLNet_TCP_Open(&server_ip);
    server_mutex = SDL_CreateMutex();
}

void Server_Receiver_deinit() {
    ByteQueue_deinit(&queue);
    SDL_DestroyMutex(queue_mutex);

    SDLNet_TCP_Close(server);
}

static int running = 1;
#define PACKET_SIZE 64

// Each client who connects gets a thread running this function
static int client_fun(void* arg) {
    int id = *((int*) arg);
    printf("Starting thread for client %d\n", id);

    TCPsocket client = shared_pool.clients[id].socket;
    SDL_mutex* client_mutex = shared_pool.clients[id].mutex;
    Client* this_client = &shared_pool.clients[id];

    SDLNet_SocketSet client_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(client_set, client);

    //temp buffer to store messages received
    Uint8 temp[PACKET_SIZE];
    while(running) {
        SDL_LockMutex(shared_pool.running_mutex);
            running = shared_pool.running;
        SDL_UnlockMutex(shared_pool.running_mutex);
        int numrecv = 0;
        SDL_LockMutex(client_mutex);
            if (SDLNet_CheckSockets(client_set, 0) > 0) {
                if (SDLNet_SocketReady(client)) {
                    numrecv = SDLNet_TCP_Recv(client, temp, PACKET_SIZE);
                }
            }
        SDL_UnlockMutex(client_mutex);

        // This is where we actually put the message in the queue
        if (numrecv) {
            Uint8 id_array[4];
            SDLNet_Write32(id, id_array);

            SDL_LockMutex(this_client->queue_mutex);
                ByteQueue_insert(this_client->queue, id_array, 4);
                ByteQueue_insert(this_client->queue, temp, numrecv);
            SDL_UnlockMutex(this_client->queue_mutex);
        }

        SDL_LockMutex(shared_pool.running_mutex);
            running = shared_pool.running;
        SDL_UnlockMutex(shared_pool.running_mutex);
    }

    printf("Stopping thread for client %d\n", id);
    return 0;
} 

//This function just listens for new connections on the server's port
static int listen_fun(void* arg) {
    if (arg != NULL) {
        printf("Warning, non-null argument passed to listen_fun\n");
    }
    printf("Starting listening thread\n");
    while(running) {
        //get the number of clients
        SDL_LockMutex(shared_pool.num_clients_mutex);
            Uint32 num_clients = shared_pool.num_clients;
        SDL_UnlockMutex(shared_pool.num_clients_mutex);

        TCPsocket client = SDLNet_TCP_Accept(server);
        if (!client) {
            SDL_Delay(100);
            continue;
        }

        //send the client their id
        Uint8 id_message[4];
        SDLNet_Write32(num_clients, id_message);
        SDLNet_TCP_Send(client, id_message, 4);

        //increment num_clients
        SDL_LockMutex(shared_pool.num_clients_mutex);
            shared_pool.num_clients++;
        SDL_UnlockMutex(shared_pool.num_clients_mutex);

        //add this client to the array
        Client* this_client = &shared_pool.clients[num_clients];
        this_client->mutex = SDL_CreateMutex();
        this_client->queue_mutex = SDL_CreateMutex();
        this_client->queue = malloc(sizeof(ByteQueue));
        ByteQueue_init(this_client->queue, QUEUE_SIZE);
        SDL_LockMutex(this_client->mutex);
            this_client->socket = client;
        SDL_UnlockMutex(this_client->mutex);
        
        //allocate a name for the thread
        client_thread_names[num_clients] = malloc(128);
        sprintf(client_thread_names[num_clients], "client_thread_%u", num_clients);

        //finally create the thread for this client
        client_threads[num_clients] = SDL_CreateThread(
                client_fun, 
                client_thread_names[num_clients], 
                &this_client->id
        );
    }

    SDL_LockMutex(shared_pool.num_clients_mutex);
        int num_clients = shared_pool.num_clients;
    SDL_LockMutex(shared_pool.num_clients_mutex);

    for(int i = 0; i < num_clients; i++) {
        int ret;
        SDL_WaitThread(client_threads[i], &ret);
    }

    printf("Stopping listening thread\n");
    return 0;
}

void Server_Receiver_run() {
    //starts the listen thread which accepts clients
    listen_thread= SDL_CreateThread(listen_fun, "listener_thread", NULL);
}

void Server_Receiver_stop() {
    int value;
    SDL_WaitThread(listen_thread, &value);
    printf("Value is %d\n", value);
}
