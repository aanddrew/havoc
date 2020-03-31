#ifndef PLAYER_H
#define PLAYER_H

#include "../graphics/Dolly.h"
#include "Vector2d.h"

typedef struct {
    Vector2d pos;
    Dolly* sprite;
    float speed;
} Player;

void Player_init(Player* self);

void Player_translate(Player* self, float dx, float dy);

#endif
