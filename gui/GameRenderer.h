#include "renderers/ProjectileRenderer.h"
#include "renderers/PlayerRenderer.h"

int GameRenderer_init(SDL_Renderer* renderer);
int GameRenderer_deinit();

void GameRenderer_render(SDL_Renderer* renderer, Camera* cam);
