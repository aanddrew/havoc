#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>

#include "Player.h"

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
    int binds[NUM_BINDS];
    int pressed[NUM_BINDS];
} Controller;

void Controller_init(Controller* self);
void Controller_keydown(Controller* self, int key);
void Controller_keyup(Controller* self, int key);
void Controller_update(Controller* self, float dt);

#endif
