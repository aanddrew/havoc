#include "Projectile.h"
#include "Vector2d.h"

static Projectile* projectiles;
static short num_projectiles = 0;
static short size_projectiles = 128;

Dolly projectile_sprites[NUM_SPRITES];

void Proj_init_all_sprites(SDL_Renderer* window_renderer) {
    Dolly_init_with_sprites(
            &projectile_sprites[ZAP],
            window_renderer,
            "res/projectile/zap_16_00.bmp",
            1
    );
}

void Proj_cleanup_all_sprites() {
    for(int i = 0; i < NUM_SPRITES; i++) {
        Dolly_delete(&projectile_sprites[i]);
    }
}

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
  self->sprite->angle = Vector2d_angle(self->dir) + -3.0f * M_PI / 4.0f;
}

void Proj_update(Projectile* self, float dt) {
    self->pos.x += self->dir.x * self->speed * dt;
    self->pos.y += self->dir.y * self->speed * dt;
    Proj_update_sprite(self);
    self->time_alive += dt;
}

Projectile* launch_proj(int kind, Vector2d pos, Vector2d dir)
{
    //logic for resizing the array
    if (!projectiles) {
        projectiles = (Projectile*) malloc(sizeof(Projectile) * size_projectiles);
    }
    if (num_projectiles == size_projectiles) {
        size_projectiles *= 2;
        projectiles = realloc(projectiles, sizeof(Projectile) * size_projectiles);
    }

    //grab the most recent open slot from our array
    Projectile* self = &projectiles[num_projectiles];
    num_projectiles++;

    //now actually spawn it
    Proj_init(self, pos, dir);
    self->sprite = &projectile_sprites[kind];
    self->kind = kind;
    Proj_update_sprite(self);
    return self;
}

void Proj_update_all(float dt) {
    for(int i = 0; i < num_projectiles; i++) {
        Proj_update(&projectiles[i], dt);
    }
}

void Proj_render_all(SDL_Renderer* renderer, const Camera* cam) {
    for(int i = 0; i < num_projectiles; i++) {
        Proj_update_sprite(&projectiles[i]);
        Dolly_render(projectiles[i].sprite, renderer, cam);
    }
}
