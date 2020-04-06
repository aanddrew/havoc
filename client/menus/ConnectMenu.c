#include "ConnectMenu.h"
#include "TextBox.h"
#include "Button.h"

#include "../../SDL_FontCache/SDL_FontCache.h"

FC_Font* font = NULL;
TTF_Font* ttf_font;
SDL_Color font_color = { 255, 255, 255 };
TextBox ip_box;

static const char* button_text[] = {
	"back",
	"join"
};

static Button buttons[CONNECTMENU_NUM_BUTTONS];

void ConnectMenu_init(SDL_Renderer* renderer) {
	font = FC_CreateFont();
    FC_LoadFont(font, renderer, "res/fonts/5ceta_mono.ttf", 20, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
	TextBox_init(&ip_box, "type ip", font);
	int w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);
	ip_box.x = w / 2 - 100;
	ip_box.y = h / 2 - 40;

	ttf_font = TTF_OpenFont("res/fonts/5ceta_mono.ttf", 14);
	for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
		Button_init_text(&buttons[i], renderer, ttf_font, button_text[i], font_color);
		buttons[i].rect.x = (w / 2) - 90+ 90 * i;
		buttons[i].rect.y = h / 2 + 50;
	}
}

void ConnectMenu_deinit() {
	for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
		Button_deinit(&buttons[i]);
	}
	TextBox_deinit(&ip_box);
	FC_FreeFont(font);
	font = NULL;
}

void ConnectMenu_event(SDL_Event e) {
	switch (e.type) {
	case SDL_KEYDOWN: {
		const char* key_name = SDL_GetKeyName(e.key.keysym.sym);
		if (strlen(key_name) == 1)
			TextBox_append_char(&ip_box, key_name[0]);
		switch (e.key.keysym.sym) {
		case SDLK_SPACE:
			TextBox_append_char(&ip_box, ' ');
			break;
		case SDLK_BACKSPACE:
			TextBox_delete_end(&ip_box);
			break;
		}
	} break;
	case SDL_MOUSEBUTTONDOWN:

		break;
	}
}

void ConnectMenu_render(SDL_Renderer* renderer) {
	TextBox_render(&ip_box, renderer);

	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);
	for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
		if (Button_is_mouse_inside(&buttons[i], mousex, mousey)) {
			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
			SDL_RenderFillRect(renderer, &buttons[i].rect);
		}
		Button_render(&buttons[i], renderer);
	}
}

void ConnectMenu_getip(char* buf, int len) {
	if (strlen(ip_box.buffer) >= len) return;
	for (int i = 0; i < strlen(ip_box.buffer) + 1; i++) {
		buf[i] = ip_box.buffer[i];
	}
}

int ConnectMenu_pressed_button(int x, int y) {
	for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
		if (Button_is_mouse_inside(&buttons[i], x, y)) {
			return i;
		}
	}
	return -1;
}
