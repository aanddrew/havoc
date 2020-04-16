#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#include "../game/Player.h"
#include "../game/Projectile.h"
#include "../utils/Vector.h"
#include "../utils/Network_utils.h"

void Network_init();
void Network_deinit();

void Network_connect(const char* hostname);
void Network_disconnect();

Vector* Network_get_received();
void Network_send_packet(ENetPacket* packet);

Uint32 Network_get_our_id();

int Network_online();
int Network_error();

#endif
