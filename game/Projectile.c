#include "Projectile.h"
#include "Vector2d.h"

static Projectile* projectiles;
static short num_projectiles = 0;
static short size_projectiles = 128;

static void Proj_init_range(int start, int end)
{
    for (int i = start; i < end; i++) {
        Proj_init(&projectiles[i], Vector2d_zero, Vector2d_zero);
    }
}

void Proj_init_all()
{
    projectiles = malloc(sizeof(Projectile) * size_projectiles);
    Proj_init_range(0, size_projectiles);
    num_projectiles = 0;
}

void Proj_init(Projectile* self, Vector2d pos, Vector2d dir)
{
    self->pos = pos;
    self->dir = Vector2d_normalize(dir);
    self->speed = 1000;
    self->time_alive = 0;
    self->max_time_alive = 0.5f;

    self->hitbox.center.x = self->pos.x;
    self->hitbox.center.y = self->pos.y;
    self->hitbox.radius = 32;

    self->team = -1;

    self->is_alive = 0;
    self->is_allocated = 0;
}

static void Proj_update(Projectile* self, float dt)
{
    self->pos.x += self->dir.x * self->speed * dt;
    self->pos.y += self->dir.y * self->speed * dt;
    self->time_alive += dt;

    self->hitbox.center.x = self->pos.x;
    self->hitbox.center.y = self->pos.y;

    if (self->time_alive >= self->max_time_alive) {
        self->is_alive = 0;
    }
}

void Proj_update_all(float dt)
{
    for (int i = 0; i < num_projectiles; i++) {
        if (projectiles[i].is_alive)
            Proj_update(&projectiles[i], dt);
    }
}

//launch projectile at given index
Projectile* Proj_launch_at_index(int kind, Vector2d pos, Vector2d dir, int team, int index)
{
    //logic for resizing the array
    if (!projectiles) {
        projectiles = (Projectile*)malloc(sizeof(Projectile) * size_projectiles);
    }
    while (index >= size_projectiles) {
        size_projectiles *= 2;
        projectiles = realloc(projectiles, sizeof(Projectile) * size_projectiles);
    }

    Projectile* self = &projectiles[index];

    //now actually spawn it
    Proj_init(self, pos, dir);
    self->kind = kind;
    self->is_alive = 1;
    self->team = team;
    return self;
}

//automatically find index and launch projectile
Projectile* Proj_launch(int kind, Vector2d pos, Vector2d dir, int team, int* index)
{
    int temp_index = -1;

    for (int i = 0; i < num_projectiles; i++) {
        if (!projectiles[i].is_alive) {
            temp_index = i;
            break;
        }
    }
    if (temp_index == -1) {
        temp_index = num_projectiles;
        num_projectiles++;
    }

    if (index) {
        *index = temp_index;
    }

    Projectile* self = Proj_launch_at_index(kind, pos, dir, team, temp_index);
    return self;
}

int Proj_num_projectiles()
{
    return num_projectiles;
}

Projectile* Proj_get(int index)
{
    if (index >= num_projectiles || index < 0) {
        return NULL;
    }
    if (!projectiles[index].is_alive) {
        return NULL;
    }
    return &projectiles[index];
}
