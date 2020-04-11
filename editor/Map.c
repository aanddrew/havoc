#include "Map.h"

#include <stdio.h>
#include <math.h>

#include <SDL2/SDL_image.h>

void Map_init(Map* self, const char* file_name) {
    self->width = 16;
    self->height = 16;
    self->tiles = malloc(self->width * self->height * sizeof(Uint16));

    if (file_name == NULL) {
        for(int x = 0; x < self->width; x++) {
            for(int y = 0; y < self->height; y++) {
                Map_set_tile(self, rand() % 12, x, y);
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
    //if (type < 0 || type >= NUM_TILES) return;
    if (out_of_bounds(self, x, y)) return;

    self->tiles[(x * self->height) + y] = type;
}

Uint16 Map_get_tile(Map* self, int x, int y) {
    return self->tiles[x * self->height + y];
}

void Map_save(Map* self, const char* file_name) {
    FILE* file = fopen(file_name, "wb");
    if (!file) {
        printf("Error writing to file: %s\n", file_name);
        return;
    }
    fwrite(&self->width, 4, 1, file);
    fwrite(&self->height, 4, 1, file);
    fwrite(self->tiles, self->width * self->height * sizeof(Uint16), 1, file);
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
