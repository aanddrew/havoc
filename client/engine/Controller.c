#include "Controller.h"

static const int default_binds[] = {
    SDLK_w,
    SDLK_s,
    SDLK_a,
    SDLK_d,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
};

void Controller_init(Controller* self) {
    self->player = NULL;
    for(int i = 0; i < NUM_BINDS; i++) {
        self->binds[i] = default_binds[i];
        self->pressed[i] = 0;
    }
}

static int key_to_bind(Controller* self, int key) {
    for(int i = 0; i < NUM_BINDS; i++) {
        if (self->binds[i] == key) {
            return i;
        }
    }
    return -1;
}

void Controller_keydown(Controller* self, int key) {
    self->pressed[key_to_bind(self, key)] = 1;
}
void Controller_keyup(Controller* self, int key) {
    self->pressed[key_to_bind(self, key)] = 0;
}

void Controller_update(Controller* self, float dt) {
    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    Vector2d mouse_pos = {mousex, mousey};
    Vector2d mouse_diff = Vector2d_subtract(mouse_pos, self->player->pos);
    float angle = Vector2d_angle(mouse_diff);
    self->player->sprite->angle = angle;

    self->player->look = mouse_diff;

    float dx = 0.0f;
    float dy = 0.0f;
    if (self->pressed[UP]) { dy = -1 * self->player->speed * dt; }
    if (self->pressed[DOWN]) { dy = self->player->speed * dt; }
    if (self->pressed[LEFT]) { dx = -1 * self->player->speed * dt; }
    if (self->pressed[RIGHT]) { dx = self->player->speed * dt; }

    Player_translate(self->player, dx, dy);
}
