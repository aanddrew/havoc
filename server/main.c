#include <SDL2/SDL.h>
#include <enet/enet.h>
#include <stdio.h>

#include "../game/Game.h"
#include "../utils/Network_utils.h"

ENetPeer* peers[16];
ENetHost* server = NULL;
void handle_packet(int id, ENetPacket* pack);

static Vector* sending;
static Vector* sending_swap;
static SDL_mutex* sending_mutex;

int game_fun(void* args);

int main()
{
    if (SDL_Init(0)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
    }
    if (enet_initialize() != 0) {
        printf("An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);
    //done initializing enet

    //initialize our global variables
    for (int i = 0; i < 16; i++) {
        peers[i] = NULL;
    }

    ENetAddress address;
    ENetHost* server = NULL;

    address.host = ENET_HOST_ANY;
    address.port = 23432;

    server = enet_host_create(&address, 512, 2, 0, 0);

    if (!server) {
        printf("ERROR creating ENet host\n");
        return EXIT_FAILURE;
    }

    sending = malloc(sizeof(Vector));
    sending_swap = malloc(sizeof(Vector));
    Vector_init(sending);
    Vector_init(sending_swap);
    sending_mutex = SDL_CreateMutex();

    //now we start polling events

    ENetEvent event;
    while (1) {
        int val = enet_host_service(server, &event, 10);
        if (val > 0) {
            //HANDLE SERVER RECEIVING PACKETS
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                int id;
                Player_connect("default", &id);
                if (id < 0) {
                    printf("Client tried to connect, but server was full!\n");
                    break;
                }

                peers[id] = event.peer;

                event.peer->data = malloc(sizeof(int));
                *((int*)event.peer->data) = id;

                //send them a packet with their id
                int id_data[2];
                id_data[0] = GIVE_ID;
                id_data[1] = id;
                ENetPacket* id_pack = enet_packet_create(id_data, 8, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, id_pack);

                printf("New client %d connected!\n", id);
            } break;
            case ENET_EVENT_TYPE_RECEIVE: {
                int id = *((int*)event.peer->data);

                printf("Packet received from client %d: ", id);
                print_packet(event.packet);

                enet_uint8* buf = malloc(event.packet->dataLength);
                for (int i = 0; i < (int)event.packet->dataLength; i++) {
                    buf[i] = event.packet->data[i];
                }
                handle_packet(id, event.packet);

            } break;
            case ENET_EVENT_TYPE_DISCONNECT: {
                int id = *((int*)event.peer->data);

                Player_disconnect(id);

                printf("client %d disconnected.\n", id);
                free(event.peer->data);
            } break;
            default:
                break;
            }
        }

        //SEND PACKETS OUT TO CLIENTS
        SDL_LockMutex(sending_mutex);
        Vector* temp = sending;
        sending = sending_swap;
        sending_swap = temp;
        SDL_UnlockMutex(sending_mutex);

        Vector_reverse(sending_swap);
        while (sending_swap->num > 0) {
            printf("sending\n");
            ENetPacket* pack = Vector_pop(sending_swap);
            enet_host_broadcast(server, 0, pack);
        }
        /*
        for(int i = 0; i < 16; i++) {
            if (peers[i] == NULL) continue;

            const char* message = "hello, client!";
            ENetPacket *packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peers[i], 0, packet);
        }
        */
    }

    enet_host_destroy(server);
    SDL_Quit();
}

void handle_packet(int id, ENetPacket* pack)
{
    if (pack->dataLength < 4) {
        return;
    }

    int type = *((int*)pack->data);

    switch (type) {
    case PLAYER_UPDATE:
        Network_decipher_player_packet(pack, id, 1);
        break;
    }
    enet_packet_destroy(pack);
}

int game_fun(void* args)
{
    if (args) {
        printf("Error, game fun should not take args\n");
    }

    Game_init();

    int done = 0;
    int last_time = SDL_GetTicks();
    while (!done) {
        last_time = SDL_GetTicks();
        int dt_ms = SDL_GetTicks() - last_time;
        float dt = ((float)dt_ms) / 1000.0f;
        Game_update(dt);

        SDL_LockMutex(sending_mutex);
        //send data from players in game
        for (int i = 0; i < Player_num_players(); i++) {
            if (!Player_get(i))
                continue;

            ENetPacket* player_pack = Network_create_player_packet(Player_get(i), i);
            Vector_push(sending, player_pack);
        }
        SDL_UnlockMutex(sending_mutex);
    }

    Game_deinit();

    return 0;
}
