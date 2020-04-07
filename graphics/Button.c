#include "Button.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
	SDL_Surface* surface = TTF_RenderText_Solid(font, msg, color);
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

void Button_init_icon(Button* self, const char* img_file) {

}

void Button_render(Button* self, SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, self->texture, NULL, &self->rect);
}

int Button_is_mouse_inside(Button* self, int x, int y) {
	return (x >= self->rect.x) &&
		(x <= self->rect.x + self->rect.w) &&
		(y >= self->rect.y) &&
		(y <= self->rect.y + self->rect.h);
}