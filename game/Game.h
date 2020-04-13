#ifndef GAME_H
#define GAME_H

#include "Projectile.h"
#include "Player.h"
#include "Map.h"
#include "Vector2d.h"

void Game_init();
void Game_deinit();

void Game_update(float dt);

#endif
