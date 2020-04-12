#include "Game.h"

void Game_init()
{
    Proj_init_all();
}

void Game_deinit()
{
}

void Game_update(float dt)
{
    Proj_update_all(dt);
    Player_update_all(dt);
}
