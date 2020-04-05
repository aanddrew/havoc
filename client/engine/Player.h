#ifndef PLAYER_H
#define PLAYER_H

#include "../graphics/Dolly.h"
#include "Vector2d.h"

typedef struct {
    Vector2d pos;
    Vector2d look;
    Dolly* sprite;
    float speed;
} Player;

void Player_init(Player* self);
void Player_init_wizard(Player* self, SDL_Renderer* renderer);

void Player_update_sprite(Player* self);

void Player_translate(Player* self, float dx, float dy);

#endif
