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
    SDL_Surface* surface; //not sure if we need to hold onto this
    SDL_Texture* texture;

    SDL_Rect rect; //change w and h to change how large it is drawn on screen
    SDL_Rect srcrect; //change w and h to change how large it is in texture

    float angle; 
    int offset; // how much to separate each layer by vertically
} Dolly;

void Dolly_init(Dolly* self);
void Dolly_init_with_texture(Dolly* self, SDL_Renderer* window_renderer, const char* file_name);
void Dolly_refresh_texture_from_surface(Dolly* self, SDL_Renderer* window_renderer);

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
