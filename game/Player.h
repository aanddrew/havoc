#ifndef PLAYER_H
#define PLAYER_H

#include "../gui/Dolly.h"
#include "Vector2d.h"
#include "HitBox.h"

typedef struct {
    Vector2d pos;
    Vector2d vel;
    Vector2d look;
    float speed;

    HitBox hitbox;
    int team;

    char* name;

    float health;

    int is_alive;
    int is_connected;
} Player;

void Player_init(Player* self);
void Player_deinit(Player* self);

void Player_init_all();

//moves the player based on their velocity and dt
void Player_update(Player* self, float dt);
void Player_translate(Player* self, float dx, float dy);

void Player_deal_damage(Player* self, float dmg);

void Player_update_all(float dt);

Player* Player_connect_with_id(char* name, int id);
Player* Player_connect(char* name, int* id);
void Player_disconnect(int id);
void Player_set_name(char* name, int id);

//gets the number of players in the game
int Player_num_players();
//gets a player of certain id in the game
Player* Player_get(int index);

#endif
