#include "Game.h"

//static const int MAX_PLAYERS = 16;
#define MAX_PLAYERS 16
static Player players[MAX_PLAYERS];

void Game_init() {
    Proj_init_all();
}

void Game_deinit() {
    
}

void Game_update(float dt) {
    Proj_update_all(dt);
    Player_update_all(dt);
}
