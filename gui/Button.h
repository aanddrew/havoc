#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
	SDL_Rect rect;
	SDL_Texture* texture;

    SDL_Rect srcrect;

    int is_active;
    int is_hovered;
    int is_hidden;
} Button;

void Button_init(Button* self);
void Button_deinit(Button* self);
void Button_init_text(Button* self, SDL_Renderer* renderer, TTF_Font* font, const char* msg, SDL_Color color);
void Button_init_icon(Button* self, SDL_Renderer* renderer, const char* img_file);
void Button_init_texture(Button* self, SDL_Texture* texture);

void Button_render(Button* self, SDL_Renderer* renderer);

void Button_get_screen_coords(Button*self, SDL_Renderer* renderer, int* x, int* y);
int Button_is_mouse_inside(Button* self, SDL_Renderer* renderer, int x, int y);

#endif
