#include "Map.h"

#include <math.h>

static Dolly tile_dollys[NUM_TILES];
static int num_maps_initted = 0;

static const char* tile_files[] = {
    "../res/map/dirt.bmp",
    "../res/map/stone.bmp",
    "../res/map/water.bmp",
};

#define TILE_WIDTH 16

void Map_init(Map* self, SDL_Renderer* renderer, const char* file_name) {
    if (num_maps_initted == 0) {
        for(int i = 0; i < NUM_TILES; i++) {
            Dolly_init_with_sprites(&tile_dollys[i], renderer, tile_files[i], 1);
            tile_dollys[i].rect.w = TILE_WIDTH;
            tile_dollys[i].rect.h = TILE_WIDTH;
        }
    }
    self->width = 64;
    self->height = 64;
    self->tiles = malloc(self->width * sizeof(Uint8*));
    for(int x = 0; x < self->width; x++) {
        self->tiles[x] = malloc(self->height);
    }

    if (file_name == NULL) {
        for(int x = 0; x < self->width; x++) {
            for(int y = 0; y < self->height; y++) {
                self->tiles[x][y] = rand() % NUM_TILES;
            }
        }
    }
    else {
        //load map from file
    }
}

void Map_deinit(Map* self) {
    for(int x = 0; x < self->width; x++) {
        free(self->tiles[x]);
    }
    free(self->tiles);
}

int out_of_bounds(Map* self, int x, int y) {
    if (x < 0 || x >= self->width || y < 0 || y >= self->height)
        return 1;
    return 0;
}

void Map_set_tile(Map* self, int type, int x, int y) {
    if (type < 0 || type >= NUM_TILES) return;
    if (out_of_bounds(self, x, y)) return;

    self->tiles[x][y] = type;
}

void Map_render(Map* self, SDL_Renderer* renderer, const Camera* cam) {
    for(int x = 0; x < self->width; x++) {
        for(int y = 0; y < self->height; y++) {
            tile_dollys[self->tiles[x][y]].rect.x = TILE_WIDTH * x;
            tile_dollys[self->tiles[x][y]].rect.y = TILE_WIDTH * y;
            Dolly_render(&tile_dollys[self->tiles[x][y]], renderer, cam);
        }
    }
}

