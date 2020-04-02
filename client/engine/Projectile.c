#include "Projectile.h"
#include "Vector2d.h"

static Projectile* projectiles;
static short num_projectiles = 0;
static short size_projectiles = 128;

static Vector* projectile_sprites;

void Proj_init(Projectile* self, Vector2d pos, Vector2d dir)
{
  self->pos = pos;
  self->dir = Vector2d_normalize(dir);
  self->speed = 1000;
  self->sprite = NULL;
}

static void Proj_update_sprite(Projectile* self)
{
  self->sprite->rect.x = (int) (self->pos.x);
  self->sprite->rect.y = (int) (self->pos.y);
  self->sprite->angle = Vector2d_angle(self->dir);
}

void Proj_update(Projectile* self, float dt) {
    self->pos.x += self->dir.x * self->speed * dt;
    self->pos.y += self->dir.y * self->speed * dt;
    Proj_update_sprite(self);
}

void launch_proj(Dolly* sprite, int kind, Vector2d pos, Vector2d dir)
{
    if (!projectiles) {
        projectiles = (Projectile*) malloc(sizeof(Projectile) * size_projectiles);
    }
    if (num_projectiles == size_projectiles) {
        size_projectiles *= 2;
        projectiles = realloc(projectiles, sizeof(Projectile) * size_projectiles);
    }

    Projectile* self = &projectiles[num_projectiles];
    num_projectiles++;

    Proj_init(self, pos, dir);
    self->sprite = sprite;
    Proj_update_sprite(self);
}

void Proj_update_all(float dt) {
    for(int i = 0; i < num_projectiles; i++) {
        Proj_update(&projectiles[i], dt);
    }
}

void Proj_render_all(SDL_Renderer* renderer) {
    for(int i = 0; i < num_projectiles; i++) {
        Proj_update_sprite(&projectiles[i]);
        Dolly_render(projectiles[i].sprite, renderer);
    }
}
