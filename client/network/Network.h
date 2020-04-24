#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#include "../../game/Player.h"
#include "../../game/Projectile.h"

void Network_init();
void Network_deinit();

int Network_connect(const char *hostname);
void Network_disconnect();

void Network_send_packet(UDPpacket *packet);

Uint32 Network_get_our_id();

int Network_online();

#endif
