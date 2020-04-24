#ifndef FONTS_H
#define FONTS_H

#include "../SDL_FontCache/SDL_FontCache.h"

void Fonts_init(SDL_Renderer *renderer);
void Fonts_deinit();

FC_Font *Fonts_getfont(int size);

#endif
