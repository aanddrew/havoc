#ifndef HAVOC_MAP_H
#define HAVOC_MAP_H

#include <SDL2/SDL.h>

typedef struct {
    Uint16* tiles;
    int width;
    int height;
} Map;

void Map_init(Map* self, const char* file_name);
void Map_deinit(Map* self);

void Map_set_tile(Map* self, int type, int x, int y);
Uint16 Map_get_tile(Map* self, int x, int y);

void Map_save(Map* self, const char* file_name);
void Map_load(Map* self, const char* file_name);

#endif
