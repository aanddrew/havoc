#include "Map.h"

#include <math.h>
#include <stdio.h>

void Map_init(Map *self, const char *file_name) {
  if (file_name) {
    self->tiles = NULL;
    Map_load(self, file_name);
    return;
  }

  self->tile_width = 64;
  self->width = 16;
  self->height = 16;
  self->depth = 4;
  self->tiles =
      malloc(self->width * self->height * self->depth * sizeof(Uint16));

  for (int z = 0; z < self->depth; z++) {
    for (int x = 0; x < self->width; x++) {
      for (int y = 0; y < self->height; y++) {
        Map_set_tile(self, rand() % 12, x, y, z);
      }
    }
  }
  for (int i = 0; i < 8; i++) {
    self->team_spawns[i][0] = -1;
    self->team_spawns[i][1] = -1;
  }
}

void Map_deinit(Map *self) { free(self->tiles); }

int out_of_bounds(Map *self, int x, int y, int z) {
  if (x < 0 || x >= self->width || y < 0 || y >= self->height || z < 0 ||
      z >= self->depth)
    return 1;
  return 0;
}

void Map_set_tile(Map *self, Uint16 type, int x, int y, int z) {
  // if (type < 0 || type >= NUM_TILES) return;
  if (out_of_bounds(self, x, y, z))
    return;

  self->tiles[(z * self->height * self->width) + (x * self->height) + y] = type;
}

Uint16 Map_get_tile(Map *self, int x, int y, int z) {
  if (out_of_bounds(self, x, y, z))
    return 0;

  return self->tiles[(z * self->height * self->width) + x * self->height + y];
}

void Map_get_spawn(Map *self, int team, int *x, int *y) {
  if (team < 0 || team >= 8) {
    printf("Map_get_spawn: team %d not defined\n", team);
    *x = -1;
    *y = -1;
    return;
  }
  *x = self->team_spawns[team][0];
  *y = self->team_spawns[team][1];
}

void Map_save(Map *self, const char *file_name) {
  FILE *file = fopen(file_name, "wb");
  if (!file) {
    printf("Error writing to file: %s\n", file_name);
    return;
  }
  fwrite(&self->width, 4, 1, file);
  fwrite(&self->height, 4, 1, file);
  fwrite(&self->depth, 4, 1, file);
  fwrite(self->tiles, self->width * self->height * self->depth * sizeof(Uint16),
         1, file);
  fwrite(self->team_spawns, 16 * sizeof(int), 1, file);
  fclose(file);
}

void Map_load(Map *self, const char *file_name) {
  FILE *file = fopen(file_name, "rb");
  if (!file) {
    printf("Error reading from file: %s\n", file_name);
    return;
  }

  self->tile_width = 64;
  fread(&self->width, 4, 1, file);
  fread(&self->height, 4, 1, file);
  fread(&self->depth, 4, 1, file);
  int num_tiles = self->width * self->height * self->depth;

  if (self->tiles != NULL) {
    free(self->tiles);
  }
  self->tiles = malloc(sizeof(Uint16) * num_tiles);
  for (int i = 0; i < num_tiles; i++) {
    self->tiles[i] = 0;
  }

  fread(self->tiles, num_tiles * sizeof(Uint16), 1, file);
  fread(self->team_spawns, 16 * sizeof(int), 1, file);
  fclose(file);
}

void Map_collide_player(Map *self, Player *player) {
  if (player->pos.x >= self->width * self->tile_width) {
    player->pos.x = self->width * self->tile_width - 1;
    player->vel.x = 0;
  } else if (player->pos.x < 0) {
    player->pos.x = 1;
    player->vel.x = 0;
  }

  if (player->pos.y >= self->height * self->tile_width) {
    player->pos.y = self->height * self->tile_width - 1;
    player->vel.y = 0;
  } else if (player->pos.y < 0) {
    player->pos.y = 1;
    player->vel.y = 0;
  }
}
