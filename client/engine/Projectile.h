#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../graphics/Dolly.h"
#include "../graphics/Camera.h"
#include "Vector2d.h"
#include "Player.h"

typedef struct {
    Dolly* sprite;
    Vector2d pos; 
    Vector2d dir; 
    float speed;
    float time_alive;
} Projectile;

enum PROJECTILE_SPRITES {
    ZAP,
    NUM_SPRITES,
};

void Proj_init_all_sprites(SDL_Renderer* window_renderer);
void Proj_cleanup_all_sprites();

void Proj_init(Projectile* self, Vector2d pos, Vector2d dir);

void launch_proj(int kind, Vector2d pos, Vector2d dir);

void Proj_render_all(SDL_Renderer* renderer, const Camera* cam);
void Proj_update_all(float dt);

#endif
