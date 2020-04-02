#ifndef SERVER_H
#define SERVER_H

#include "../client/engine/Player.h"

typedef struct {
    Player players[2];
} GameState;

extern GameState Master_GameState;

void Server_init(short port);
void Server_deinit();

void Server_start();
void Server_stop();

#endif
