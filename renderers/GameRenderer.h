#include "PlayerRenderer.h"
#include "ProjectileRenderer.h"

int GameRenderer_init(SDL_Renderer *renderer);
int GameRenderer_deinit();

void GameRenderer_render(SDL_Renderer *renderer, Camera *cam);
