#include "GameRenderer.h"

int GameRenderer_init(SDL_Renderer* renderer) {
    Proj_init_all_sprites(renderer);
    Player_init_all_sprites(renderer);
}

int GameRenderer_deinit() {
}

void GameRenderer_render(SDL_Renderer* renderer, Camera* cam) {
    Proj_render_all(renderer, cam);
    Player_render_all(renderer, cam);
}
