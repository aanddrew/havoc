#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} Window;

Window* Window_init();
void Window_clear(Window* self);
void Window_present(Window* self);
void Window_delete(Window* self);

#endif
