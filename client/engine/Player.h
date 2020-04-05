#ifndef PLAYER_H
#define PLAYER_H

#include "../graphics/Dolly.h"
#include "Vector2d.h"

typedef struct {
    Vector2d pos;
    Vector2d vel;
    Vector2d look;
    Dolly* sprite;
    float speed;
} Player;

void Player_init(Player* self);
void Player_init_wizard(Player* self, SDL_Renderer* renderer);

//moves the player based on their velocity, also calls update sprite
void Player_update(Player* self, float dt);
//updates sprite so it is on top of the player and faces correctly
void Player_update_sprite(Player* self);

void Player_translate(Player* self, float dx, float dy);

#endif
