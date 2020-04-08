#include "Button.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

void Button_init(Button* self) {
	self->rect.x = 0;
	self->rect.y = 0;
	self->rect.w = 0;
	self->rect.h = 0;
	self->texture = NULL;
}

void Button_deinit(Button* self) {
	if (self->texture) {
		SDL_DestroyTexture(self->texture);
	}
}

void Button_init_text(Button* self, SDL_Renderer* renderer, 
	TTF_Font* font, const char* msg, SDL_Color color) {

    Button_init(self);
	SDL_Surface* surface = TTF_RenderText_Solid(font, msg, color);
    if (!surface) {
        printf("Error in Button_init_text: %s\n", SDL_GetError());
    }
	self->texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!self->texture) {
		printf("Error: %s\n", SDL_GetError());
	}
	SDL_FreeSurface(surface);
	int w, h;
	TTF_SizeText(font, msg, &w, &h);
	self->rect.w = w;
	self->rect.h = h;
}

void Button_init_icon(Button* self, SDL_Renderer* renderer, const char* img_file) {
    Button_init(self);

    SDL_Surface* surface = IMG_Load(img_file);
    if (!surface) {
        printf("Error in Button_init_icon: %s\n", SDL_GetError());
    }
    self->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!surface || !self->texture) {
        printf("Error loading image in Button_init_icon: %s\n" ,SDL_GetError());
    }
    SDL_FreeSurface(surface);
    self->rect.w = 64;
    self->rect.h = 64;
}

void Button_render(Button* self, SDL_Renderer* renderer) {
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    SDL_Rect temp_rect;
    temp_rect.x = self->rect.x + (self->rect.x < 0 ? screen_w : 0);
    temp_rect.y = self->rect.y + (self->rect.y < 0 ? screen_h : 0);
    temp_rect.w = self->rect.w;
    temp_rect.h = self->rect.h;
	SDL_RenderCopy(renderer, self->texture, NULL, &temp_rect);
}

int Button_is_mouse_inside(Button* self, int x, int y) {
	return (x >= self->rect.x) &&
		(x <= self->rect.x + self->rect.w) &&
		(y >= self->rect.y) &&
		(y <= self->rect.y + self->rect.h);
}
