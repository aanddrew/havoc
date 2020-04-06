#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../../SDL_FontCache/SDL_FontCache.h"

#define TEXTBOX_BUFFER_SIZE 512
typedef struct {
	char buffer[TEXTBOX_BUFFER_SIZE];
	const char* placeholder;
	FC_Font* font;

	int x;
	int y;

	int num_chars;
} TextBox;

void TextBox_init(TextBox* self, const char* placeholder, FC_Font* font);
void TextBox_deinit(TextBox* self);

void TextBox_insert(TextBox* self, const char* text, int start);
void TextBox_append(TextBox* self, const char* text);
void TextBox_set(TextBox* self, char c, int index);
void TextBox_append_char(TextBox* self, char c);
void TextBox_delete_end(TextBox* self);

void TextBox_render(TextBox* self, SDL_Renderer* renderer);

#endif