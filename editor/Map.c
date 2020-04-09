#include "Map.h"

#include <stdio.h>
#include <math.h>

static Dolly tile_dollys[NUM_TILES];
static int num_maps_initted = 0;

const char* tile_files[] = {
    "../res/map/dirt.png",
    "../res/map/stone.png",
    "../res/map/water.png",
    "../res/map/sand.png",
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
    self->height = 128;
    self->tiles = malloc(self->width * sizeof(Uint8) * self->height * sizeof(Uint8));

    if (file_name == NULL) {
        for(int x = 0; x < self->width; x++) {
            for(int y = 0; y < self->height; y++) {
                Map_set_tile(self, rand() % NUM_TILES, x, y);
            }
        }
    }
    else {
        //load map from file
    }
}

void Map_deinit(Map* self) {
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

    self->tiles[(x * self->height) + y] = type;
}

Uint8 Map_get_tile(Map* self, int x, int y) {
    return self->tiles[x * self->height + y];
}

void Map_render(Map* self, SDL_Renderer* renderer, const Camera* cam) {
    for(int x = 0; x < self->width; x++) {
        for(int y = 0; y < self->height; y++) {
            Uint8 tile = Map_get_tile(self, x, y);
            tile_dollys[tile].rect.x = TILE_WIDTH * x;
            tile_dollys[tile].rect.y = TILE_WIDTH * y;
            Dolly_render(&tile_dollys[tile], renderer, cam);
        }
    }
}

void Map_save(Map* self, const char* file_name) {
    FILE* file = fopen(file_name, "wb");
    if (!file) {
        printf("Error writing to file: %s\n", file_name);
        return;
    }
    fwrite(&self->width, 4, 1, file);
    fwrite(&self->height, 4, 1, file);
    fwrite(self->tiles, self->width * self->height, 1, file);
    fclose(file);
}

void Map_load(Map* self, const char* file_name) {
    FILE* file = fopen(file_name, "rb");
    if (!file) {
        printf("Error writing to file: %s\n", file_name);
        return;
    }
    fread(&self->width, 4, 1, file);
    fread(&self->height, 4, 1, file);
    fread(self->tiles, self->width * self->height, 1, file);
    fclose(file);

}
