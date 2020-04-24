#include "Button.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Fonts.h"

void Button_init(Button *self) {
  self->rect.x = 0;
  self->rect.y = 0;
  self->rect.w = 0;
  self->rect.h = 0;

  self->centerx = 0;
  self->centery = 0;

  self->type = TEXTURE;
  self->texture = NULL;
  self->is_active = 0;
  self->is_hovered = 0;
  self->is_hidden = 0;

  self->bg_color.r = 200;
  self->bg_color.g = 200;
  self->bg_color.b = 200;
  self->bg_color.a = 255;
  self->draw_background = 0;
}

void Button_deinit(Button *self) {
  switch (self->type) {
  case TEXTURE:
    if (self->texture) {
      SDL_DestroyTexture(self->texture);
    }
    break;
  case TEXT:
    if (self->text) {
      free(self->text);
    }
    break;
  }
}

void Button_init_text(Button *self, const char *msg, int font_size) {
  Button_init(self);

  self->type = TEXT;
  self->text = strdup(msg);
  self->font_size = font_size;

  FC_Font *font = Fonts_getfont(self->font_size);

  self->rect.w = FC_GetWidth(font, self->text);
  self->rect.h = FC_GetLineHeight(font);

  self->srcrect.x = 0;
  self->srcrect.y = 0;

  self->is_active = 0;
  self->is_hovered = 0;
  self->is_hidden = 0;
  self->is_dummy = 0;
}

void Button_init_icon(Button *self, SDL_Renderer *renderer,
                      const char *img_file) {
  SDL_Surface *surface = IMG_Load(img_file);
  if (!surface) {
    printf("Error in Button_init_icon: %s\n", SDL_GetError());
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
  if (!surface || !self->texture) {
    printf("Error loading image in Button_init_icon: %s\n", SDL_GetError());
  }
  Button_init_texture(self, tex);
  SDL_FreeSurface(surface);
}

void Button_init_texture(Button *self, SDL_Texture *tex) {
  Button_init(self);
  self->type = TEXTURE;
  self->texture = tex;
  self->rect.w = 64;
  self->rect.h = 64;

  self->srcrect.x = 0;
  self->srcrect.y = 0;
  self->srcrect.w = 64;
  self->srcrect.h = 64;

  self->is_active = 0;
  self->is_hovered = 0;
  self->is_hidden = 0;
  self->is_dummy = 0;
}

void Button_render(Button *self, SDL_Renderer *renderer) {
  if (self->is_hidden)
    return;
  int screen_w, screen_h;
  SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

  SDL_Rect temp_rect;
  Button_get_screen_coords(self, renderer, &temp_rect.x, &temp_rect.y);
  temp_rect.w = self->rect.w;
  temp_rect.h = self->rect.h;

  if (self->draw_background) {
    SDL_SetRenderDrawColor(renderer, self->bg_color.r, self->bg_color.g,
                           self->bg_color.b, self->bg_color.a);
    SDL_RenderFillRect(renderer, &temp_rect);
  }

  switch (self->type) {
  case TEXTURE: {
    if (self->texture)
      SDL_RenderCopy(renderer, self->texture, &self->srcrect, &temp_rect);
  } break;
  case TEXT:
    FC_Draw(Fonts_getfont(self->font_size), renderer, temp_rect.x, temp_rect.y,
            self->text);
    break;
  }

  if (self->is_hovered) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
    SDL_RenderFillRect(renderer, &temp_rect);
  }

  if (self->is_active) {
    SDL_Color border_color = {255, 255, 255, 255};
    SDL_Rect copy = self->rect;
    for (int i = 0; i < 3; i++) {
      SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g,
                             border_color.b, border_color.a);
      SDL_RenderDrawRect(renderer, &copy);
      copy.x++;
      copy.y++;
      copy.w -= 2;
      copy.h -= 2;
      border_color.r -= 50;
      border_color.g -= 50;
      border_color.b -= 50;
    }
  }
}

void Button_get_screen_coords(Button *self, SDL_Renderer *renderer, int *x,
                              int *y) {
  int screen_w, screen_h;
  SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

  int offsetx = 0;
  if (self->centerx) {
    offsetx = screen_w / 2;
  } else if (self->rect.x < 0) {
    offsetx = screen_w;
  }

  int offsety = 0;
  if (self->centery) {
    offsety = screen_h / 2;
  } else if (self->rect.y < 0) {
    offsety = screen_h;
  }
  *x = self->rect.x + offsetx;
  *y = self->rect.y + offsety;
}

int Button_is_mouse_inside(Button *self, SDL_Renderer *renderer, int x, int y) {
  int self_x, self_y;
  Button_get_screen_coords(self, renderer, &self_x, &self_y);

  return (x >= self_x) && (x <= self_x + self->rect.w) && (y >= self_y) &&
         (y <= self_y + self->rect.h);
}
