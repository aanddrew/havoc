#include "Network_utils.h"

#include <enet/enet.h>

void print_packet(const ENetPacket* pack)
{
    for (int i = 0; i < (int)pack->dataLength; i++) {
        if (i != 0 && i % 4 == 0)
            printf(" ");
        printf("%02x", ((int)pack->data[i]));
    }
    printf("\n");
}

ENetPacket* Network_create_player_packet(Player* player, int id)
{
    enet_uint8 data[36];
    *((int*)data) = PLAYER_UPDATE;
    *((int*)data + 4) = id;

    float* float_arr = (float*)(data + 8);
    float_arr[0] = player->pos.x;
    float_arr[1] = player->pos.y;

    float_arr[2] = player->vel.x;
    float_arr[3] = player->vel.y;

    float_arr[4] = player->look.x;
    float_arr[5] = player->look.y;

    float_arr[6] = player->health;

    ENetPacket* pack = enet_packet_create(data, 36, 0);
    return pack;
}

void Network_decipher_player_packet(ENetPacket* pack, int id, int server)
{
    if (pack->dataLength != 36) {
        printf("INVALID PLAYER PACKET, length of %lu\n", pack->dataLength);
        return;
    }
    Player* player = Player_get(id);
    if (!player)
        return;

    float* float_arr = (float*)pack->data + 8;

    player->pos.x = float_arr[0];
    player->pos.y = float_arr[1];

    player->vel.x = float_arr[2];
    player->vel.y = float_arr[3];

    player->look.x = float_arr[4];
    player->look.y = float_arr[5];

    if (!server) {
        player->health = float_arr[6];
    }
}

//we only care about receiving health from the server right now
void Network_decipher_own_player_packet(ENetPacket* pack, int id)
{
    if (pack->dataLength != 36) {
        printf("INVALID PLAYER PACKET, length of %lu\n", pack->dataLength);
        return;
    }

    Player* player = Player_get(id);
    player->health = ((float*)pack->data)[6];
}

/*
UDPpacket* Network_create_player_die_packet(int id)
{
    UDPpacket* pack = SDLNet_AllocPacket(12);
    SDLNet_Write32(-1, pack->data);
    SDLNet_Write32(PLAYER_DIE, pack->data + 4);
    SDLNet_Write32(id, pack->data + 8);
    pack->len = 12;
    return pack;
}

void Network_decipher_player_die_packet(UDPpacket* pack)
{
    if (pack->len != 12) {
        printf("INVALID PLAYER DIE PACKET, length of %d\n", pack->len);
        return;
    }
    int id = SDLNet_Read32(pack->data + 8);
    Player* p = Player_get(id);
    if (p) {
        p->is_alive = 0;
    }
}

UDPpacket* Network_create_player_spawn_packet(int id, Player* player)
{
    UDPpacket* pack = SDLNet_AllocPacket(20);
    SDLNet_Write32(id, pack->data);
    SDLNet_Write32(PLAYER_SPAWN, pack->data + 4);

    Uint32 x = *((Uint32*)&player->pos.x);
    Uint32 y = *((Uint32*)&player->pos.y);
    Uint32 h = *((Uint32*)&player->health);
    SDLNet_Write32(x, pack->data + 8);
    SDLNet_Write32(y, pack->data + 12);
    SDLNet_Write32(h, pack->data + 16);
    pack->len = 20;
    return pack;
}

void Network_decipher_player_spawn_packet(UDPpacket* pack)
{
    if (pack->len != 20) {
        printf("INVALID SPAWN PACKET of length %d\n", pack->len);
        return;
    }
    int id = SDLNet_Read32(pack->data);
    Uint32 int_x = SDLNet_Read32(pack->data + 8);
    float x = *((float*)&int_x);
    Uint32 int_y = SDLNet_Read32(pack->data + 12);
    float y = *((float*)&int_y);

    Uint32 int_h = SDLNet_Read32(pack->data + 16);
    float h = *((float*)&int_h);

    Player_get(id)->is_alive = 1;
    Player_get(id)->pos.x = x;
    Player_get(id)->pos.y = y;
    Player_get(id)->health = h;
}

UDPpacket* Network_create_projectile_packet(Projectile* proj)
{
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

void Network_decipher_projectile_packet(UDPpacket* pack, Projectile* proj)
{
    if (proj) {
        printf("Error, proj parameter is not used at the moment\n");
    }
    Uint32 id = SDLNet_Read32(pack->data);

    Uint32 kind = SDLNet_Read32(pack->data + 8);

    Uint32 my_x = SDLNet_Read32(pack->data + 12);
    Uint32 my_y = SDLNet_Read32(pack->data + 16);
    Uint32 my_dir_x = SDLNet_Read32(pack->data + 20);
    Uint32 my_dir_y = SDLNet_Read32(pack->data + 24);

    Vector2d pos;
    pos.x = *((float*)&my_x);
    pos.y = *((float*)&my_y);
    Vector2d dir;
    dir.x = *((float*)&my_dir_x);
    dir.y = *((float*)&my_dir_y);

    Proj_launch((int)kind, pos, dir, Player_get(id)->team, NULL);
}

UDPpacket* Network_create_projectile_death_packet(int id)
{
    UDPpacket* pack = SDLNet_AllocPacket(12);
    SDLNet_Write32(-1, pack->data);
    SDLNet_Write32(PROJECTILE_DEATH, pack->data + 4);
    SDLNet_Write32(id, pack->data + 8);
    pack->len = 12;
    return pack;
}

void Network_decipher_projectile_death_packet(UDPpacket* pack)
{
    if (pack->len != 12) {
        printf("INVALID LENGTH FOR PROJECTILE DECIPHER PACKET\n");
    }
    Uint32 id = SDLNet_Read32(pack->data + 8);
    Projectile* p = Proj_get((int)id);
    if (p) {
        p->is_alive = 0;
    }
}

#define MAX_NAME_SIZE 32
UDPpacket* Network_create_change_name_packet(const char* name)
{
    UDPpacket* pack = SDLNet_AllocPacket(4 + MAX_NAME_SIZE);
    if (strlen(name) >= MAX_NAME_SIZE) {
        pack->len = 4 + MAX_NAME_SIZE;
    } else {
        pack->len = 4 + strlen(name) + 1;
    }
    SDLNet_Write32(CHANGE_NAME, pack->data);

    int i;
    for (i = 0; i < (int)(strlen(name)) && i < MAX_NAME_SIZE - 1; i++) {
        pack->data[i + 4] = name[i];
    }
    pack->data[i + 4] = '\0';

    return pack;
}

void Network_decipher_change_name_packet(UDPpacket* pack)
{
    int id = SDLNet_Read32(pack->data);

    char buffer[MAX_NAME_SIZE];
    int i = 0;
    for (char* c = (char*)(pack->data + 8); *c != '\0' && i < MAX_NAME_SIZE - 1; c++) {
        buffer[i] = *c;
        i++;
    }
    buffer[i] = '\0';
    Player_set_name(buffer, id);
}

UDPpacket* Network_create_get_names_packet()
{
    UDPpacket* pack = SDLNet_AllocPacket(4);
    pack->len = 4;
    SDLNet_Write32(GET_NAMES, pack->data);
    return pack;
}

void Network_decipher_get_names_packet(UDPpacket* pack) {
    
}

UDPpacket* Network_create_receive_names_packet()
{
    UDPpacket* pack = SDLNet_AllocPacket(MAX_NAME_SIZE * 10 + 8 + 10);

    SDLNet_Write32(-1, pack->data);
    SDLNet_Write32(RECEIVE_NAMES, pack->data + 4);

    char* head = (char*)(pack->data + 8);
    for (int i = 0; i < Player_num_players(); i++) {
        Player* p = Player_get(i);
        if (p) {
            SDLNet_Write32(i, (Uint32*)head);
            head += 4;
            for (int j = 0; j < (int)(strlen(p->name)); j++) {
                *head = p->name[j];
                head++;
            }
            *head = 0;
            head++;
        }
    }
    pack->len = (int)(((Uint8*)head) - pack->data) + 1;
    return pack;
}

void Network_decipher_receive_names_packet(UDPpacket* pack)
{
    char* head = (char*)(pack->data + 8);
    char* end = (char*)(pack->data + pack->len);
    while (head <= end - 4) {
        int id = SDLNet_Read32(head);
        head += 4;

        char name[MAX_NAME_SIZE];
        int i = 0;
        while (*head != '\0' && i < MAX_NAME_SIZE - 1) {
            name[i] = *head;
            head++;
            i++;
        }
        head++; //skips head past null character, otherwise next id is WRONG
        name[i] = '\0';

        Player_set_name(name, id);
    }
}
*/
