#include "Network_utils.h"

UDPpacket* Network_create_player_packet(Player* player) {
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

void Network_decipher_projectile_packet(UDPpacket* pack, Projectile* proj) {
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

    Proj_launch((int) kind, pos, dir, NULL);
}
