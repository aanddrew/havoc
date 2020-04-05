#include "Player.h"
#include "Vector2d.h"

void Player_init(Player* self) {
    self->pos.x = 0.0f;
    self->pos.y = 0.0f;
    self->speed = 200.0f;
    self->sprite = NULL;
}

void Player_init_wizard(Player* self, SDL_Renderer* renderer) {
    Player_init(self);
    Dolly* wiz = malloc(sizeof(Dolly));
    Dolly_init_with_sprites(wiz, renderer, "res/wizard/wizard_16_00.bmp", 9);
    self->sprite = wiz;
}

void Player_update(Player* self, float dt) {
    Player_translate(self, self->vel.x * dt, self->vel.y * dt);
    Player_update_sprite(self);
}

void Player_update_sprite(Player* self) {
    self->sprite->rect.x = (int) (self->pos.x) - self->sprite->rect.w/2;
    self->sprite->rect.y = (int) (self->pos.y) - self->sprite->rect.h/2;
    float angle = Vector2d_angle(self->look);
    self->sprite->angle = angle;
}

void Player_translate(Player* self, float dx, float dy) {
    self->pos.x += dx;
    self->pos.y += dy;
}
