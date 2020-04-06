#include "MainMenu.h"
#include "Button.h"

#include <SDL2/SDL_ttf.h>
#include "../../SDL_FontCache/SDL_FontCache.h"

const char* button_text[] = {
	"connect",
	"options",
	"quit"
};

static Button buttons[NUM_BUTTONS];
static Button havoc_button;

TTF_Font* big_font = NULL;
TTF_Font* small_font = NULL;
SDL_Color textColor = { 255, 255, 255 };

SDL_Rect font_rect;

void MainMenu_init(SDL_Renderer* renderer) {
	big_font = TTF_OpenFont("res/fonts/5ceta_mono.ttf", 24);
	small_font = TTF_OpenFont("res/fonts/5ceta_mono.ttf", 14);
	if (!big_font) {
		printf("Error opening main menu font: %s\n", SDL_GetError());
	}

	int start_y = 200;
	for (int i = 0; i < NUM_BUTTONS; i++) {
		Button_init_text(&buttons[i], renderer, small_font, button_text[i], textColor);
		buttons[i].rect.x = 50;
		buttons[i].rect.y = start_y;
		start_y += buttons[i].rect.h + 10;
	}
	Button_init_text(&havoc_button, renderer, big_font, "HAVOC", textColor);
	havoc_button.rect.x = 50;
	havoc_button.rect.y = 100;
}

void MainMenu_deinit() {
	TTF_CloseFont(big_font);
	TTF_CloseFont(small_font);
}

SDL_Texture* message_texture = NULL;
void MainMenu_Render(SDL_Renderer* renderer) {
	int mousex, mousey;
	SDL_GetMouseState(&mousex, &mousey);
	for (int i = 0; i < NUM_BUTTONS; i++) {
		if (Button_is_mouse_inside(&buttons[i], mousex, mousey)) {
			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
			SDL_RenderFillRect(renderer, &buttons[i].rect);
		}
		Button_render(&buttons[i], renderer);
	}
	Button_render(&havoc_button, renderer);
}

int MainMenu_pressed_button(int x, int y) {
	for (int i = 0; i < NUM_BUTTONS; i++) {
		if (Button_is_mouse_inside(&buttons[i], x, y)) {
			return i;
		}
	}
	return -1;
}
