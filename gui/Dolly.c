#include "Dolly.h"
#include "../global.h"

#include <SDL2/SDL_image.h>

#include "Camera.h"
#include <string.h>

void Dolly_init(Dolly *self) {
  self->rect.x = 0;
  self->rect.y = 0;
  self->rect.w = 64;
  self->rect.h = 64;

  self->srcrect.x = 0;
  self->srcrect.y = 0;

  self->surface = NULL;
  self->texture = NULL;

  self->offset = 5;
}

void Dolly_init_with_texture(Dolly *self, SDL_Renderer *window_renderer,
                             const char *file_name) {
  Dolly_init(self);
  self->surface = IMG_Load(file_name);
  self->texture = SDL_CreateTextureFromSurface(window_renderer, self->surface);
  if (!self->surface || !self->texture) {
    printf("Error loading texture file %s: %s\n", file_name, SDL_GetError());
    return;
  }
}

void Dolly_team_colorize(Dolly *self, SDL_Renderer *renderer, int team) {
  int num_pixels = self->surface->w * self->surface->h;
  // set the colors in the surface to reflect the team of this wizard
  for (int p = 0; p < num_pixels; p++) {
    Uint8 *pixel = self->surface->pixels + (p * 4);
    SDL_Color *color = (SDL_Color *)pixel;
    if (color->r > 128 && color->g > 128 && color->b > 128) {
      continue;
    }

    // team color is determined by the bits in the team number
    // i.e. team 5 is team 0b101 which would have the red and green bits
    // colored, making them the yellow team
    int new_r = (team & 1) ? 200 : color->r;
    int new_g = (team & 2) ? 150 : color->g;
    int new_b = (team & 4) ? 200 : color->b;
    color->r = new_r;
    color->g = new_g;
    color->b = new_b;
  }

  Dolly_refresh_texture_from_surface(self, renderer);
}

void Dolly_refresh_texture_from_surface(Dolly *self,
                                        SDL_Renderer *window_renderer) {
  if (self->texture)
    SDL_DestroyTexture(self->texture);

  self->texture = SDL_CreateTextureFromSurface(window_renderer, self->surface);
}

void Dolly_render(Dolly *self, SDL_Renderer *window_renderer,
                  const Camera *cam) {
  SDL_Rect rect_copy = self->rect;
  SDL_Rect camera_rect;

  int x = 0;
  int y = 0;

  int w, h;
  SDL_QueryTexture(self->texture, NULL, NULL, &w, &h);

  while (y <= h - self->srcrect.h) {
    while (x <= w - self->srcrect.w) {
      Camera_transform_rect(cam, &rect_copy, &camera_rect);

      self->srcrect.x = x;
      self->srcrect.y = y;

      SDL_RenderCopyEx(window_renderer, self->texture, &self->srcrect,
                       &camera_rect, (double)(self->angle) * 180.0f / M_PI,
                       NULL, SDL_FLIP_NONE);

      x += self->srcrect.w;
      rect_copy.y -= self->offset;
    }
    y += self->srcrect.h;
  }
}

void Dolly_setPos(Dolly *self, int x, int y) {
  self->rect.x = x;
  self->rect.y = y;
}
void Dolly_translate(Dolly *self, int dx, int dy) {
  self->rect.x += dx;
  self->rect.y += dy;
}

void Dolly_setAngle(Dolly *self, float angle) { self->angle = angle; }
void Dolly_rotate(Dolly *self, float dTheta) { self->angle += dTheta; }

int Dolly_getX(Dolly *self) { return self->rect.x; }
int Dolly_getY(Dolly *self) { return self->rect.y; }

void Dolly_delete(Dolly *self) {
  SDL_FreeSurface(self->surface);
  SDL_DestroyTexture(self->texture);
}
