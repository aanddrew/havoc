#ifndef MAINMENU_H
#define MAINMENU_H

#include <SDL2/SDL.h>

enum buttons {
	CONNECT_BUTTON,
	OPTIONS_BUTTON,
	QUIT_BUTTON,
	NUM_BUTTONS,
};

void MainMenu_init(SDL_Renderer* renderer);
void MainMenu_deinit();

void MainMenu_Render(SDL_Renderer* render);

int MainMenu_pressed_button(int x, int y);

#endif