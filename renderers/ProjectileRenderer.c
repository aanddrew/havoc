#include "ProjectileRenderer.h"

#include "../game/Projectile.h"
#include "../gui/Dolly.h"

Dolly projectile_sprites[NUM_PROJECTILE_KINDS];
int projectile_sizes[NUM_PROJECTILE_KINDS] = {
    64,
};

void Proj_init_all_sprites(SDL_Renderer* window_renderer)
{
    Dolly_init_with_sprites(
        &projectile_sprites[ZAP],
        window_renderer,
        "res/projectile/zap_16_00.bmp",
        1);
}

void Proj_cleanup_all_sprites()
{
    for (int i = 0; i < NUM_PROJECTILE_KINDS; i++) {
        Dolly_delete(&projectile_sprites[i]);
    }
}

static Dolly* get_sprite(int kind)
{
    if (kind >= NUM_PROJECTILE_KINDS || kind < 0) {
        return NULL;
    }
    return &projectile_sprites[kind];
}

static void Proj_update_sprite(const Projectile* self)
{
    if (!self)
        return;
    Dolly* dolly = get_sprite(self->kind);
    if (!dolly)
        return;

    dolly->rect.x = (int)(self->pos.x) - 32;
    dolly->rect.y = (int)(self->pos.y) - 32;
    dolly->angle = Vector2d_angle(self->dir) + -3.0f * M_PI / 4.0f;
}

void Proj_render_all(SDL_Renderer* renderer, const Camera* cam)
{
    int num = Proj_num_projectiles();
    for (int i = 0; i < num; i++) {
        const Projectile* proj = Proj_get(i);
        if (!proj)
            continue;

        Proj_update_sprite(proj);
        Dolly_render(get_sprite(proj->kind), renderer, cam);
    }
}
