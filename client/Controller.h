#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>

#include "../game/Player.h"
#include "../gui/Camera.h"

//
// Updates a player object based on inputs from SDL
//

//indeces of the binds and pressed array which correspond to certain actions
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
    CENTER_CAMERA,
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

//does not actually update the player, just changes the look vector and velocity
void Controller_update(Controller* self, float dt, const Camera* cam);

#endif
