#include "MapRenderer.h"

#include "../gui/Atlas.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char *texture_file = "res/map/havoc_textures.png";
const char *spawner_texture_file = "res/wizard/havoc_spawner.png";
Dolly spawners[8];

Atlas map_atlas;

void MapRenderer_init(SDL_Renderer *renderer) {
  Atlas_init_texture(&map_atlas, renderer, texture_file, 64);

  for (int i = 0; i < 8; i++) {
    Dolly_init_with_texture(&spawners[i], renderer, spawner_texture_file);
    Dolly_team_colorize(&spawners[i], renderer, i);
    spawners[i].srcrect.w = 64;
    spawners[i].srcrect.h = 64;
  }
}

void MapRenderer_deinit() { Atlas_deinit(&map_atlas); }

void Map_render(Map *self, SDL_Renderer *renderer, const Camera *cam) {
  for (int z = 0; z < self->depth; z++) {
    for (int x = 0; x < self->width; x++) {
      for (int y = 0; y < self->height; y++) {
        Uint16 tile = Map_get_tile(self, x, y, z);
        Atlas_render(&map_atlas, renderer, tile, x * map_atlas.tile_width,
                     y * map_atlas.tile_width, map_atlas.tile_width,
                     map_atlas.tile_width, cam);
      }
    }
  }

  for (int i = 0; i < 8; i++) {
    int x, y;
    Map_get_spawn(self, i, &x, &y);
    if (x < 0 || y < 0) {
      continue;
    }

    x *= map_atlas.tile_width;
    y *= map_atlas.tile_width;
    spawners[i].rect.x = x;
    spawners[i].rect.y = y;
    Dolly_render(&spawners[i], renderer, cam);
  }
}

SDL_Texture *MapRenderer_gettexture() { return map_atlas.texture; }

int MapRenderer_get_texture_width() { return map_atlas.surface->w; }

int MapRenderer_get_texture_height() { return map_atlas.surface->h; }
