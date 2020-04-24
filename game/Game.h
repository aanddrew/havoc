#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "Projectile.h"
#include "Vector2d.h"

void Game_init();
void Game_deinit();

void Game_update(float dt);

Map *Game_getmap();

#endif
