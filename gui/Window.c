#include "Window.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

Window *Window_init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return NULL;
  }
  if (TTF_Init() < 0) {
    printf("Error with TTF_Init(): %s\n", SDL_GetError());
    return NULL;
  }
  int flags = IMG_INIT_JPG | IMG_INIT_PNG;
  if (IMG_Init(flags) != flags) {
    printf("Error with IMG_Init: %s\n", SDL_GetError());
    return NULL;
  }

  Window *window = malloc(sizeof(Window));

  window->window =
      SDL_CreateWindow("havoc", 100, 100, 1280, 720, SDL_WINDOW_RESIZABLE);

  // change these to disable/enable vsync
  int win_flags = SDL_RENDERER_PRESENTVSYNC;
  // int win_flags = 0;
  window->renderer = SDL_CreateRenderer(window->window, -1, win_flags);
  SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

  return window;
}

void Window_clear(Window *self) {
  SDL_SetRenderDrawColor(self->renderer, 100, 100, 200, 255);
  SDL_RenderClear(self->renderer);
}

void Window_present(Window *self) { SDL_RenderPresent(self->renderer); }

void Window_delete(Window *self) {
  SDL_DestroyWindow(self->window);
  SDL_DestroyRenderer(self->renderer);
  free(self);

  TTF_Quit();
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_Quit();
}
