#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "../graphics/Dolly.h"
#include "../graphics/Camera.h"
#include "Vector2d.h"
#include "Player.h"

//
// In this class there is a static array of projectile objects,
// this way projectiles are global and nobody owns them
//

typedef struct {
    Dolly* sprite;
    int kind;
    Vector2d pos; 
    Vector2d dir; 
    float speed;
    float time_alive;
} Projectile;

enum PROJECTILE_SPRITES {
    ZAP,
    NUM_SPRITES,
};

//init sprites from the sprite data base
void Proj_init_all_sprites(SDL_Renderer* window_renderer);
void Proj_cleanup_all_sprites();

//initialize a generic Projectile object, not connected to the internal system
void Proj_init(Projectile* self, Vector2d pos, Vector2d dir);

//this will create a projectile in the private projectiles array
Projectile* launch_proj(int kind, Vector2d pos, Vector2d dir);

//these do stuff to every projectile in the private array
void Proj_render_all(SDL_Renderer* renderer, const Camera* cam);
void Proj_update_all(float dt);

#endif
