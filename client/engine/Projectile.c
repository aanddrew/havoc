#include "Projectile.h"
#include "Vector2d.h"

void Proj_init(Projectile* self, Player* start)
{
  self->pos.x = start->pos.x;
  self->pos.y = start->pos.y;
  self->speed = 5;
  self->sprite = NULL;
  self->parent = start;
}

static void Proj_update_sprite(Projectile* self)
{
  self->sprite->rect.x = (int) (self->pos.x);
  self->sprite->rect.y = (int) (self->pos.y);
}

void Proj_update(Projectile* self) {
    self->pos.x += self->dir.x * self->speed;
    self->pos.y += self->dir.y * self->speed;
    Proj_update_sprite(self);
}
void launch_proj(Projectile* self)
{
    self->pos.x = self->parent->pos.x;
    self->pos.y = self->parent->pos.y;
    Proj_update_sprite(self);

    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    Vector2d mouse_pos = {mousex, mousey};
    Vector2d mouse_diff = Vector2d_subtract(mouse_pos, self->parent->pos);
    float angle = Vector2d_angle(mouse_diff);
    self->sprite->angle = angle;

    //normalize vector
    mouse_diff.x =  mouse_diff.x/(Vector2d_magnitude(mouse_diff));
    mouse_diff.y =  mouse_diff.y/(Vector2d_magnitude(mouse_diff));
    self->dir = mouse_diff;
}
