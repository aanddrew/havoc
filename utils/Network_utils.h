#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <SDL2/SDL_net.h>

#include "../game/Player.h"
#include "../game/Projectile.h"

enum MESSAGE_TYPE {
    //connection
    CONNECT_REQUEST,
    DISCONNECT_REQUEST,

    PLAYER_UPDATE,

    //names
    CHANGE_NAME,
    GET_NAMES,
    RECEIVE_NAMES,

    //projectiles
    PROJECTILE_LAUNCH,
    PROJECTILE_DEATH,
};

UDPpacket* Network_create_player_packet(Player* player);
void Network_decipher_player_packet(UDPpacket* pack, Player* player);

UDPpacket* Network_create_projectile_packet(Projectile* proj);
void Network_decipher_projectile_packet(UDPpacket* pack, Projectile* proj);

UDPpacket* Network_create_change_name_packet();
void Network_decipher_change_name_packet(UDPpacket* pack);

UDPpacket* Network_create_get_names_packet();
void Network_decipher_get_names_packet(UDPpacket* pack);

UDPpacket* Network_create_receive_names_packet();
void Network_decipher_receive_names_packet(UDPpacket* pack);

#endif
