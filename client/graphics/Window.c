#include "Window.h"

Window* Window_init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return NULL;
	}
	if (!TTF_Init() == -1) {
		return NULL;
	}

    Window* window = malloc(sizeof(Window));

	window->window = SDL_CreateWindow
	(
		"havoc", 
		100, 
		100, 
		1280, 
		720, 
		SDL_WINDOW_RESIZABLE
	);
	window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_PRESENTVSYNC);

    return window;
}

void Window_clear(Window* self) {
    SDL_SetRenderDrawColor(self->renderer, 25, 25, 50, 255);
    SDL_RenderClear(self->renderer);
}

void Window_present(Window* self) {
    SDL_RenderPresent(self->renderer);
}

void Window_delete(Window* self) {
    SDL_DestroyWindow(self->window);
    SDL_DestroyRenderer(self->renderer);
    free(self);

	TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}
