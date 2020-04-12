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
    self->is_active = 0;
    self->is_hovered = 0;
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

    self->srcrect.x = 0;
    self->srcrect.y = 0;
    self->srcrect.w = w;
    self->srcrect.h = h;

    self->is_active = 0;
    self->is_hidden = 0;
}

void Button_init_icon(Button* self, SDL_Renderer* renderer, const char* img_file) {
    SDL_Surface* surface = IMG_Load(img_file);
    if (!surface) {
        printf("Error in Button_init_icon: %s\n", SDL_GetError());
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!surface || !self->texture) {
        printf("Error loading image in Button_init_icon: %s\n" ,SDL_GetError());
    }
    Button_init_texture(self, tex);
    SDL_FreeSurface(surface);
}

void Button_init_texture(Button* self, SDL_Texture* tex) {
    Button_init(self);
    self->texture = tex;
    self->rect.w = 64;
    self->rect.h = 64;

    self->srcrect.x = 0;
    self->srcrect.y = 0;
    self->srcrect.w = 64;
    self->srcrect.h = 64;

    self->is_active = 0;
    self->is_hidden = 0;
}

void Button_render(Button* self, SDL_Renderer* renderer) {
    if (self->is_hidden) return;
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    SDL_Rect temp_rect;
    temp_rect.x = self->rect.x + (self->rect.x < 0 ? screen_w : 0);
    temp_rect.y = self->rect.y + (self->rect.y < 0 ? screen_h : 0);
    temp_rect.w = self->rect.w;
    temp_rect.h = self->rect.h;

	SDL_RenderCopy(renderer, self->texture, &self->srcrect, &temp_rect);

    if (self->is_hovered) {
        SDL_SetRenderDrawColor(renderer, 0,0,0, 50);
        SDL_RenderFillRect(renderer, &temp_rect);
    }

    if (self->is_active) {
        SDL_Color border_color = { 255, 255, 255, 255 };
        SDL_Rect copy = self->rect;
        for(int i = 0; i < 3; i++) {
            SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g,border_color.b,border_color.a);
            SDL_RenderDrawRect(renderer, &copy);
            copy.x++;
            copy.y++;
            copy.w -=2;
            copy.h -=2;
            border_color.r -=50;
            border_color.g -=50;
            border_color.b -=50;
        }
    }
}

void Button_get_screen_coords(Button*self, SDL_Renderer* renderer, int* x, int* y) {
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);
    *x = self->rect.x + (self->rect.x < 0 ? screen_w : 0);
    *y = self->rect.y + (self->rect.y < 0 ? screen_h : 0);
}

int Button_is_mouse_inside(Button* self, SDL_Renderer* renderer, int x, int y) {
    int self_x, self_y;
    Button_get_screen_coords(self, renderer, &self_x, &self_y);

	return (x >= self_x) &&
		(x <= self_x + self->rect.w) &&
		(y >= self_y) &&
		(y <= self_y + self->rect.h);
}
