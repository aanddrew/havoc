#include "Atlas.h"

#include <SDL2/SDL_image.h>

void Atlas_init(Atlas *self) {
  self->surface = NULL;
  self->texture = NULL;

  self->width = 0;
  self->height = 0;
}

void Atlas_deinit(Atlas *self) {
  if (self->surface) {
    SDL_FreeSurface(self->surface);
  }
  if (self->texture) {
    SDL_DestroyTexture(self->texture);
  }
  self->surface = NULL;
  self->texture = NULL;
}

void Atlas_init_texture(Atlas *self, SDL_Renderer *renderer,
                        const char *file_name, int tile_width) {
  self->surface = IMG_Load(file_name);
  self->texture = SDL_CreateTextureFromSurface(renderer, self->surface);

  self->tile_width = tile_width;
  self->width = self->surface->w / tile_width;
  self->height = self->surface->h / tile_width;
}

void Atlas_render(Atlas *self, SDL_Renderer *renderer, int i, int x, int y,
                  int w, int h, const Camera *cam) {
  SDL_Rect srcrect;
  srcrect.w = self->tile_width;
  srcrect.h = self->tile_width;
  srcrect.x = (i % self->width) * self->tile_width;
  srcrect.y = (i / self->height) * self->tile_width;

  SDL_Rect paintrect;
  paintrect.w = w;
  paintrect.h = h;
  paintrect.x = x;
  paintrect.y = y;

  SDL_Rect destrect = paintrect;

  if (cam) {
    Camera_transform_rect(cam, &paintrect, &destrect);
  }

  SDL_RenderCopy(renderer, self->texture, &srcrect, &destrect);
}
