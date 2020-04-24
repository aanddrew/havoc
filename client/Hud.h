#ifndef HUD_H
#define HUD_H

#include "../game/Player.h"

void Hud_init();
void Hud_deinit();

void Hud_render(SDL_Renderer *renderer, Player *p);

#endif
