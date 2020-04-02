#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../graphics/Dolly.h"
#include "Vector2d.h"
#include "Player.h"

typedef struct {
    Dolly* sprite;
    Vector2d pos; 
    Vector2d dir; 
    float speed;
} Projectile;

void Proj_init(Projectile* self, Vector2d pos, Vector2d dir);

void launch_proj(Dolly* sprite, int kind, Vector2d pos, Vector2d dir);

void Proj_render_all(SDL_Renderer* renderer);
void Proj_update_all(float dt);

#endif
