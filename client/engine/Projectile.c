#include "Projectile.h"
#include "Vector2d.h"

void Proj_init(Projectile* self, Player* start)
{
  self->pos.x = start->pos.x;
  self->pos.y = start->pos.y;
  self->speed = start->speed * 5;
  //???? why
  self->sprite = NULL;
}

static void Proj_update_sprite(Projectile* self)
{
  self->sprite->rect.x = (int) (self->pos.x);
  self->sprite->rect.y = (int) (self->pos.y);
}

void Proj_update(Projectile* self, float dx, float dy) {
    self->pos.x += dx;
    self->pos.y += dy;
    Proj_update_sprite(self);
}
