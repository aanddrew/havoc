#ifndef NETWORK_H
#define NETWORK_H

#include "Client_Pool.h"

#include "../engine/Player.h"
#include "../engine/Projectile.h"

void Network_init();
void Network_deinit();

int Network_connect(const char* hostname);
void Network_disconnect();

void Network_send_packet(UDPpacket* packet);

UDPpacket* Network_create_player_packet(Player* player);
void Network_decipher_player_packet(UDPpacket* pack, Player* player);

UDPpacket* Network_create_projectile_packet(Projectile* proj);
void Network_deciper_projectile_packet(UDPpacket* pack, Projectile* proj);

Uint32 Network_get_our_id();

int Network_online();

#endif
