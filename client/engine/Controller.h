#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>

#include "Player.h"
#include "../graphics/Camera.h"

enum KEYBINDS {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SLOT_1,
    SLOT_2,
    SLOT_3,
    SLOT_4,
    SLOT_5,
    SLOT_6,
    SLOT_7,
    NUM_BINDS,
};

typedef struct {
    Player* player;
    Camera* cam;
    int binds[NUM_BINDS];
    int pressed[NUM_BINDS];
} Controller;

void Controller_init(Controller* self);

void Controller_keydown(Controller* self, int key);
void Controller_keyup(Controller* self, int key);

void Controller_mousebuttondown(Controller*self, SDL_MouseButtonEvent e);

void Controller_mousewheel(Controller*self, SDL_MouseWheelEvent e);

void Controller_update(Controller* self, float dt, const Camera* cam);

#endif
