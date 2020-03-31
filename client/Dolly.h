#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../utils/Vector.h"

typedef struct {
    Vector* surfaces;
    Vector* textures;
    SDL_Rect rect;
    float angle;
    int offset;
} Dolly;

Dolly* Dolly_init();

//in the form *xx.* where xx is the number to iterate through
int Dolly_setSprites(Dolly* self, SDL_Renderer* window_render, const char* file_name, int num);

//drawing
void Dolly_render(Dolly* self, SDL_Renderer* window_renderer);

//moving
void Dolly_setPos(Dolly* self, int x, int y);
void Dolly_translate(Dolly* self, int dx, int dy);

void Dolly_setAngle(Dolly* self, float angle);
void Dolly_rotate(Dolly* self, float dTheta);

int Dolly_getX(Dolly* self);
int Dolly_getY(Dolly* self);

void Dolly_delete(Dolly* self);

#endif
