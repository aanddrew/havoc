#ifndef PLAYERRENDERER_H
#define PLAYERRENDERER_H

#include <SDL2/SDL.h>

#include "../game/Player.h"

void Player_init_all_sprites(SDL_Renderer* renderer);
void Player_cleanup_all_sprites();
void Player_render_all(SDL_Renderer* renderer, const Camera* cam);

#endif
