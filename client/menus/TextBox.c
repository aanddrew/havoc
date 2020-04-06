#include "TextBox.h"

void TextBox_init(TextBox* self, const char* placeholder, FC_Font* font) {
	self->placeholder = placeholder;
	self->font = font;
	self->num_chars = 0;
	self->x = 0;
	self->y = 0;
}
void TextBox_deinit(TextBox* self) {}

void TextBox_insert(TextBox* self, const char* text, int start) {
	if (start + strlen(text) > TEXTBOX_BUFFER_SIZE) {
		printf("Error: inserting too many characters into text box\n");
		return;
	}
	for (int i = 0; i < strlen(text); i++) {
		self->buffer[i + start] = text[i];
	}
	self->num_chars += strlen(text);
}

void TextBox_append(TextBox* self, const char* text) {
	if (self->num_chars + strlen(text) >= TEXTBOX_BUFFER_SIZE) {
		return;
	}
	for (int i = 0; i < strlen(text); i++) {
		self->buffer[i + self->num_chars] = text[i];
	}
	self->num_chars += strlen(text);
}

void TextBox_set(TextBox* self, char c, int index) {
	if (index < 0 || index >= TEXTBOX_BUFFER_SIZE) return;

	self->buffer[index] = c;
}

void TextBox_append_char(TextBox* self, char c) {
	if (self->num_chars == TEXTBOX_BUFFER_SIZE - 1) {
		return;
	}
	self->buffer[self->num_chars] = c;
	self->num_chars++;
	self->buffer[self->num_chars] = '\0';
}

void TextBox_delete_end(TextBox* self) {
	if (self->num_chars == 0) return;
	self->buffer[self->num_chars - 1] = '\0';
	self->num_chars--;
}

const char* TextBox_gettext(TextBox* self) {
	if (self->num_chars > 0) {
		return self->buffer;
	}
	return self->placeholder;
}

void TextBox_render(TextBox* self, SDL_Renderer* renderer) {
	SDL_Color old_color = FC_GetDefaultColor(self->font);
	if (self->num_chars == 0)
		FC_SetDefaultColor(self->font, FC_MakeColor(75, 75, 75, 255));
	FC_Draw(self->font, renderer, self->x, self->y, TextBox_gettext(self));
	FC_SetDefaultColor(self->font, old_color);
}

