#include "MapRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char* texture_file = "res/map/havoc_textures.png";
const char* spawner_texture_file = "res/wizard/havoc_spawner.png";
Dolly spawners[8];

#define TILE_WIDTH 64
static SDL_Surface* atlas_surface = NULL;
static SDL_Texture* atlas_texture = NULL;
int atlas_width = 0;
int atlas_height = 0;

void MapRenderer_init(SDL_Renderer* renderer)
{
    atlas_surface = IMG_Load(texture_file);
    if (!atlas_surface) {
        printf("Error loading textures: %s\n", SDL_GetError());
    }
    atlas_width = atlas_surface->w / TILE_WIDTH;
    atlas_height = atlas_surface->h / TILE_WIDTH;
    atlas_texture = SDL_CreateTextureFromSurface(renderer, atlas_surface);

    for (int i = 0; i < 8; i++) {
        Dolly_init_with_texture(&spawners[i], renderer, spawner_texture_file);
        Dolly_team_colorize(&spawners[i], renderer, i);
        spawners[i].srcrect.w = 64;
        spawners[i].srcrect.h = 64;
    }
}

void MapRenderer_deinit()
{
    SDL_FreeSurface(atlas_surface);
    SDL_DestroyTexture(atlas_texture);
}

void Map_render(Map* self, SDL_Renderer* renderer, const Camera* cam)
{
    SDL_Rect srcrect;
    srcrect.w = self->tile_width;
    srcrect.h = self->tile_width;

    SDL_Rect paintrect;
    paintrect.w = self->tile_width;
    paintrect.h = self->tile_width;

    SDL_Rect destrect;
    destrect.w = self->tile_width;
    destrect.h = self->tile_width;

    for (int z = 0; z < self->depth; z++) {
        for (int x = 0; x < self->width; x++) {
            for (int y = 0; y < self->height; y++) {
                Uint16 tile = Map_get_tile(self, x, y, z);

                srcrect.x = (tile % atlas_width) * self->tile_width;
                srcrect.y = (tile / atlas_width) * self->tile_width;

                paintrect.x = x * self->tile_width;
                paintrect.y = y * self->tile_width;

                Camera_transform_rect(cam, &paintrect, &destrect);
                SDL_RenderCopy(renderer, atlas_texture, &srcrect, &destrect);
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        int x, y;
        Map_get_spawn(self, i, &x, &y);
        if (x < 0 || y < 0) {
            continue;
        }
        x *= self->tile_width;
        y *= self->tile_width;
        spawners[i].rect.x = x;
        spawners[i].rect.y = y;
        Dolly_render(&spawners[i], renderer, cam);
    }
}

SDL_Texture* MapRenderer_gettexture()
{
    return atlas_texture;
}

int MapRenderer_get_texture_width()
{
    return atlas_surface->w;
}

int MapRenderer_get_texture_height()
{
    return atlas_surface->h;
}
