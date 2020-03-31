#include "Player.h"
#include "Vector2d.h"

void Player_init(Player* self) {
    self->pos.x = 0.0f;
    self->pos.y = 0.0f;
    self->speed = 200.0f;
    self->sprite = NULL;
}

static void Player_update_sprite(Player* self) {
    self->sprite->rect.x = (int) (self->pos.x);
    self->sprite->rect.y = (int) (self->pos.y);
}

void Player_translate(Player* self, float dx, float dy) {
    self->pos.x += dx;
    self->pos.y += dy;
    Player_update_sprite(self);
}
