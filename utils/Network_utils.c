#include "Network_utils.h"

UDPpacket* Network_create_player_packet(Player* player)
{
    UDPpacket* pack = SDLNet_AllocPacket(64);
    //write message type
    SDLNet_Write32(PLAYER_UPDATE, pack->data);
    //write position
    Uint32 my_x = *((int*)&(player->pos.x));
    Uint32 my_y = *((int*)&(player->pos.y));
    SDLNet_Write32(my_x, pack->data + 4);
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

    SDLNet_Write32(*((Uint32*)&player->health), pack->data + 28);

    pack->len = 32;

    return pack;
}

void Network_decipher_player_packet(UDPpacket* pack, Player* player, int server)
{
    if (pack->len != 36) {
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

    if (!server) {
        Uint32 health = SDLNet_Read32(pack->data + 32);
        player->health = *((float*)&health);
    }
}

//we only care about receiving health from the server right now
void Network_decipher_own_player_packet(UDPpacket* pack, Player* player)
{
    if (pack->len != 36) {
        printf("INVALID PLAYER PACKET, length of %d\n", pack->len);
        return;
    }

    Uint32 health = SDLNet_Read32(pack->data + 32);
    player->health = *((float*)&health);
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

/*
void Network_decipher_get_names_packet(UDPpacket* pack) {
    
}
*/

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
            for (int i = 0; i < (int)(strlen(p->name) + 1); i++) {
                *head = p->name[i];
                head++;
            }
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
        name[i] = '\0';

        Player_set_name(name, id);
    }
}
