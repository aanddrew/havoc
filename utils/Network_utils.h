#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <SDL2/SDL_net.h>

#include "../game/Player.h"
#include "../game/Projectile.h"

enum MESSAGE_TYPE {
    CONNECT_REQUEST,
    DISCONNECT_REQUEST,
    PLAYER_UPDATE,
    PROJECTILE_LAUNCH,
};

UDPpacket* Network_create_player_packet(Player* player);
void Network_decipher_player_packet(UDPpacket* pack, Player* player);

UDPpacket* Network_create_projectile_packet(Projectile* proj);
void Network_decipher_projectile_packet(UDPpacket* pack, Projectile* proj);

#endif
