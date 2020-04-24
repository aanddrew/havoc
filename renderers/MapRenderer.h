#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include <SDL2/SDL.h>

#include "../game/Map.h"
#include "../gui/Camera.h"

void MapRenderer_init(SDL_Renderer *renderer);
void MapRenderer_deinit();

void Map_render(Map *self, SDL_Renderer *renderer, const Camera *cam);

SDL_Texture *MapRenderer_gettexture();
int MapRenderer_get_texture_width();
int MapRenderer_get_texture_height();

#endif
