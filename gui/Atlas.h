#ifndef ATLAS_H
#define ATLAS_H

#include <SDL2/SDL.h>
#include "Camera.h"

typedef struct {
  SDL_Surface* surface;
  SDL_Texture* texture;

  int width; 
  int height;

  int tile_width;
} Atlas;

void Atlas_init(Atlas* self);
void Atlas_deinit(Atlas* self);

void Atlas_init_texture(Atlas* self, SDL_Renderer* renderer, const char* file_name, int tile_width);

void Atlas_render(Atlas* self, SDL_Renderer* renderer, int i, int x, int y, int w, int h, const Camera* cam);

#endif
