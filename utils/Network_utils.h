#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <enet/enet.h>

#include "../game/Player.h"
#include "../game/Projectile.h"

enum MESSAGE_TYPE {
    //server stuff
    CONNECTION,
    GIVE_ID,

    //players
    PLAYER_UPDATE,
    PLAYER_DIE,
    PLAYER_SPAWN,

    //names
    CHANGE_NAME,
    GET_NAMES,
    RECEIVE_NAMES,

    //projectiles
    PROJECTILE_LAUNCH,
    PROJECTILE_DEATH,
};

//void print_packet(UDPpacket* pack);

//players
ENetPacket* Network_create_player_packet(Player* player);
void Network_decipher_player_packet(ENetPacket* pack, int id, int server);
/*
void Network_decipher_player_packet(UDPpacket* pack, Player* player, int server);
void Network_decipher_own_player_packet(UDPpacket* pack, Player* player);

UDPpacket* Network_create_player_die_packet(int id);
void Network_decipher_player_die_packet(UDPpacket* pack);
UDPpacket* Network_create_player_spawn_packet(int id, Player* player);
void Network_decipher_player_spawn_packet(UDPpacket* pack);

//projectiles
UDPpacket* Network_create_projectile_packet(Projectile* proj);
void Network_decipher_projectile_packet(UDPpacket* pack, Projectile* proj);
UDPpacket* Network_create_projectile_death_packet(int id);
void Network_decipher_projectile_death_packet(UDPpacket* pack);

//names
UDPpacket* Network_create_change_name_packet();
void Network_decipher_change_name_packet(UDPpacket* pack);
UDPpacket* Network_create_get_names_packet();
void Network_decipher_get_names_packet(UDPpacket* pack);
UDPpacket* Network_create_receive_names_packet();
void Network_decipher_receive_names_packet(UDPpacket* pack);

//server stuff
UDPpacket* Network_create_disconnect_packet();
void Network_decipher_disconnect_packet(UDPpacket* pack);
*/

#endif
