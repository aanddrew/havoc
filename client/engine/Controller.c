#include "Controller.h"

#include "Projectile.h"

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
    SDLK_SPACE,
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
    int bind = key_to_bind(self, key);
    if (bind < 0 || bind >= NUM_BINDS) return;
    self->pressed[bind] = 1;
}
void Controller_keyup(Controller* self, int key) {
    int bind = key_to_bind(self, key);
    if (bind < 0 || bind >= NUM_BINDS) return;
    self->pressed[bind] = 0;
}

void Controller_mousebuttondown(Controller*self, SDL_MouseButtonEvent e) {
    launch_proj(0, self->player->pos, self->player->look);
}

void Controller_mousewheel(Controller*self, SDL_MouseWheelEvent e) {
    int scrolled = e.y;
    Camera_zoom(self->cam, ((float) scrolled) /15.0f + 1.0f);
}

void Controller_update(Controller* self, float dt, const Camera* cam) {
    //update the look vector based on mouse position
    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);

    mousex = ((float) mousex) / cam->scale;
    mousey = ((float) mousey) / cam->scale;

    mousex += cam->x;
    mousey += cam->y;

    Vector2d mouse_pos = {mousex, mousey};
    Vector2d mouse_diff = Vector2d_subtract(mouse_pos, self->player->pos);
    self->player->look = mouse_diff;

    //update players velocity based on keys pressed
    float dx = 0.0f;
    float dy = 0.0f;
    if (self->pressed[UP]) { dy = -1 * self->player->speed; }
    if (self->pressed[DOWN]) { dy = self->player->speed; }
    if (self->pressed[LEFT]) { dx = -1 * self->player->speed; }
    if (self->pressed[RIGHT]) { dx = self->player->speed; }

    Vector2d newvel;
    newvel.x = dx;
    newvel.y = dy;
    self->player->vel = newvel;

    //center the camer if the player is pressing the center camera button
    if (self->pressed[CENTER_CAMERA]) { 
        Camera_set_center(self->cam, self->player->pos.x, self->player->pos.y); 
    }
}
