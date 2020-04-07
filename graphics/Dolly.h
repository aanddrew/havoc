#ifndef SPRITE_H
#define SPRITE_H

#include "Camera.h"

#include <SDL2/SDL.h>
#include "../utils/Vector.h"

//
// The Dolly mixture is something invented by 3kliksphilip (google it)
// It involves creating a model by stacking 2d sprites which are like
// cross sections of the intended character
//
// It gives a pseudo-3d effect if these sprites move and rotate in unison
//

typedef struct {
    Vector surfaces; //not sure if we need to hold onto these
    Vector textures;
    SDL_Rect rect;
    float angle; 
    int offset; // how much to separate each layer by vertically
} Dolly;

void Dolly_init(Dolly* self);
void Dolly_init_with_sprites(Dolly* self, SDL_Renderer* window_renderer, const char* file_name, int num);

//in the form *xx.* where xx is the number to iterate through
//This creates num layers of sprites to use in the model
int Dolly_setSprites(Dolly* self, SDL_Renderer* window_render, const char* file_name, int num);

//drawing
void Dolly_render(Dolly* self, SDL_Renderer* window_renderer, const Camera* cam);

//moving
void Dolly_setPos(Dolly* self, int x, int y);
void Dolly_translate(Dolly* self, int dx, int dy);

void Dolly_setAngle(Dolly* self, float angle);
void Dolly_rotate(Dolly* self, float dTheta);

int Dolly_getX(Dolly* self);
int Dolly_getY(Dolly* self);

void Dolly_delete(Dolly* self);

#endif
