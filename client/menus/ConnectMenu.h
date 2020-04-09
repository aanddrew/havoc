#ifndef CONNECTMENU_H
#define CONNECTMENU_H

#include <SDL2/SDL.h>

enum BUTTONS {
	CONNECTMENU_BACK_BUTTON,
	CONNECTMENU_JOIN_BUTTON,
	CONNECTMENU_NUM_BUTTONS,
};

void ConnectMenu_init(SDL_Renderer* renderer);
void ConnectMenu_deinit();

void ConnectMenu_event(SDL_Event e);
void ConnectMenu_render(SDL_Renderer* renderer);
void ConnectMenu_getip(char* buf, int len);

int ConnectMenu_pressed_button(SDL_Renderer* renderer, int x, int y);

#endif
