#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum BUTTON_TYPE {
  TEXT,
  TEXTURE,
};

typedef struct {
  SDL_Rect rect;
  int type;

  union {
    struct {
      char *text;
      int font_size;
    };
    SDL_Texture *texture;
  };

  int centerx; // when non-zero rect.x is added to half the renderer's width
  int centery; // when non-zero rect.y is added to half the renderer's height

  SDL_Color bg_color;

  SDL_Rect srcrect; // srcrect for drawing from texture

  int is_active;  // when non-zero draw a square border around the button
  int is_hovered; // when non-zero draw a gray semi-transparent square over the
                  // button
  int is_hidden;  // when non-zero don't draw the button
  int is_dummy;   // used by menus, when non-zero it should not be hovered or
                  // active
  int draw_background; // draw a background behind the texture
} Button;

void Button_init(Button *self);
void Button_deinit(Button *self);
void Button_init_text(Button *self, const char *msg, int font_size);
void Button_init_icon(Button *self, SDL_Renderer *renderer,
                      const char *img_file);
void Button_init_texture(Button *self, SDL_Texture *texture);

void Button_render(Button *self, SDL_Renderer *renderer);

void Button_get_screen_coords(Button *self, SDL_Renderer *renderer, int *x,
                              int *y);
int Button_is_mouse_inside(Button *self, SDL_Renderer *renderer, int x, int y);

#endif
