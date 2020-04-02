#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../graphics/Dolly.h"
#include "Vector2d.h"
#include "Player.h"

typedef struct {
    Player* parent;
    Dolly* sprite;
    Vector2d pos; 
    Vector2d dir; 
    float speed;

} Projectile;

void Proj_init(Projectile* self, Player* start);

void Proj_update(Projectile* self);
void launch_proj(Projectile* self);

#endif
