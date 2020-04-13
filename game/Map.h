#ifndef HAVOC_MAP_H
#define HAVOC_MAP_H

#include "../game/Player.h"

typedef struct {
    Uint16* tiles;
    int width;
    int height;
    int depth;
} Map;

void Map_init(Map* self, const char* file_name);
void Map_deinit(Map* self);

void Map_set_tile(Map* self, Uint16 type, int x, int y, int depth);
Uint16 Map_get_tile(Map* self, int x, int y, int depth);

void Map_save(Map* self, const char* file_name);
void Map_load(Map* self, const char* file_name);

void Map_collide_player(Map* self, Player* player);

#endif
