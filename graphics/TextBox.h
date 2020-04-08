#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#define TEXTBOX_BUFFER_SIZE 512
typedef struct {
	char buffer[TEXTBOX_BUFFER_SIZE];
	int num_chars;

	const char* placeholder;
	FC_Font* font;

	int x;
	int y;

    int box_width;
    int is_active;
} TextBox;

void TextBox_init(TextBox* self, const char* placeholder, FC_Font* font);
void TextBox_deinit(TextBox* self);

void TextBox_insert(TextBox* self, const char* text, int start);
void TextBox_append(TextBox* self, const char* text);
void TextBox_set(TextBox* self, char c, int index);
void TextBox_append_char(TextBox* self, char c);
void TextBox_delete_end(TextBox* self);

void TextBox_render(TextBox* self, SDL_Renderer* renderer);
void TextBox_render_bg(TextBox* self, SDL_Renderer* renderer);
void TextBox_render_bg_color(TextBox* self, SDL_Renderer* renderer, SDL_Color color);

void TextBox_get_screen_coords(TextBox*self, SDL_Renderer* renderer, int* x, int* y);

int TextBox_is_mouse_inside(TextBox* self, SDL_Renderer* renderer, int x, int y);


#endif
