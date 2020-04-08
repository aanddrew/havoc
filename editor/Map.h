#ifndef MAP_H
#define MAP_h

#include <SDL2/SDL.h>

#include "../graphics/Dolly.h"

#define MAP_WIDTH 128

typedef struct {
    Uint8** tiles;
    int width;
    int height;
} Map;

enum TILES {
    DIRT,
    STONE,
    WATER,
    SAND,
    NUM_TILES,
};

extern const char* tile_files[];

void Map_init(Map* self, SDL_Renderer* renderer, const char* file_name);
void Map_deinit(Map* self);

void Map_set_tile(Map* self, int type, int x, int y);

void Map_render(Map* self, SDL_Renderer* renderer, const Camera* cam);

#endif
