#include "Network.h"
#include "Client_Pool.h"
#include "Client_Receiver.h"
#include "Client_Sender.h"
#include "Message.h"

#include "../engine/Player.h"

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

UDPpacket* Network_create_player_packet(Player* player) {
    if (!online) return;
    UDPpacket* pack = SDLNet_AllocPacket(64);
    //write message type
    SDLNet_Write32(PLAYER_UPDATE, pack->data);
    //write position
    Uint32 my_x = *((int*)&(player->pos.x));
    Uint32 my_y = *((int*)&(player->pos.y));
    SDLNet_Write32(my_x, pack->data+ 4);
    SDLNet_Write32(my_y, pack->data + 8);
    
    //write velocity
    Uint32 my_vel_x = *((int*)&(player->vel.x));
    Uint32 my_vel_y = *((int*)&(player->vel.y));
    SDLNet_Write32(my_vel_x, pack->data + 12);
    SDLNet_Write32(my_vel_y, pack->data + 16);

    //write look vector
    Uint32 my_look_x = *((int*)&(player->look.x));
    Uint32 my_look_y = *((int*)&(player->look.y));
    SDLNet_Write32(my_look_x, pack->data + 20);
    SDLNet_Write32(my_look_y, pack->data + 24);

    pack->len = 28;

    return pack;
}

void Network_decipher_player_packet(UDPpacket* pack, Player* player) {
    if (!online) return;
    if (pack->len != 32) {
        printf("INVALID PLAYER PACKET, length of %d\n", pack->len);
        return;
    }

    //same order as it was written in 
    Uint32 x = SDLNet_Read32(pack->data + 8);
    Uint32 y = SDLNet_Read32(pack->data + 12);
    player->pos.x = *((float*)&x);
    player->pos.y = *((float*)&y);

    Uint32 vel_x = SDLNet_Read32(pack->data + 16);
    Uint32 vel_y = SDLNet_Read32(pack->data + 20);
    player->vel.x = *((float*)&vel_x);
    player->vel.y = *((float*)&vel_y);

    Uint32 look_x = SDLNet_Read32(pack->data + 24);
    Uint32 look_y = SDLNet_Read32(pack->data + 28);
    player->look.x = *((float*)&look_x);
    player->look.y = *((float*)&look_y);
}

UDPpacket* Network_create_projectile_packet(Projectile* proj) {
    if (!online) return;
    UDPpacket* pack = SDLNet_AllocPacket(64);

    SDLNet_Write32(PROJECTILE_LAUNCH, pack->data);

    SDLNet_Write32(proj->kind, pack->data + 4);

    //write position
    Uint32 my_x = *((int*)&(proj->pos.x));
    Uint32 my_y = *((int*)&(proj->pos.y));
    SDLNet_Write32(my_x, pack->data + 8);
    SDLNet_Write32(my_y, pack->data + 12);
    
    //write dir
    Uint32 my_dir_x = *((int*)&(proj->dir.x));
    Uint32 my_dir_y = *((int*)&(proj->dir.y));
    SDLNet_Write32(my_dir_x, pack->data + 16);
    SDLNet_Write32(my_dir_y, pack->data + 20);

    pack->len = 24;

    return pack;
}

void Network_deciper_projectile_packet(UDPpacket* pack, Projectile* proj) {
    if (!online) return;
    Uint32 kind = SDLNet_Read32(pack->data + 8);

    Uint32 my_x = SDLNet_Read32(pack->data + 12);
    Uint32 my_y = SDLNet_Read32(pack->data + 16);
    Uint32 my_dir_x = SDLNet_Read32(pack->data + 20);
    Uint32 my_dir_y = SDLNet_Read32(pack->data + 24);

    Vector2d pos;
    pos.x = *((float*) & my_x);
    pos.y = *((float*) & my_y);
    Vector2d dir;
    dir.x = *((float*) & my_dir_x);
    dir.y = *((float*) & my_dir_y);

    launch_proj((int) kind, pos, dir);
}

Uint32 Network_get_our_id() {
    return our_id;
}

int Network_online() {
    return online;
}