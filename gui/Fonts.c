#include "Fonts.h"

#define DEFAULT_FONT "res/fonts/5ceta_mono.ttf"

#define NUM_FONT_SIZES 64
FC_Font* fonts[NUM_FONT_SIZES];

SDL_Renderer* font_renderer;

void Fonts_init(SDL_Renderer* renderer) {
    for(int i = 0; i < NUM_FONT_SIZES; i++) {
        fonts[i] = NULL;
    }
    font_renderer = renderer;
}

void Fonts_deinit() {
    for(int i = 0; i < NUM_FONT_SIZES; i++) {
        if (fonts[i] != NULL) {
            FC_FreeFont(fonts[i]);
        }
    }
}

FC_Font* Fonts_getfont(int size) {
    if (fonts[size] == NULL) {
        fonts[size] = FC_CreateFont();
        FC_LoadFont(fonts[size], font_renderer, DEFAULT_FONT, size, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
    }
    return fonts[size];
}
