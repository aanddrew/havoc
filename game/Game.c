#include "Game.h"

static Map map;

void Game_init() {
  Proj_init_all();
  Player_init_all();
  Map_init(&map, "editor/maps/test_map.hm");
}

void Game_deinit() { Map_deinit(&map); }

Map *Game_getmap() { return &map; }

void Game_update(float dt) {
  Proj_update_all(dt);

  // now make projectiles collide with players
  for (int i = 0; i < Proj_num_projectiles(); i++) {
    for (int j = 0; j < Player_num_players(); j++) {
      if (!Player_get(j) || !Proj_get(i))
        continue;
      if (Player_get(j)->team == Proj_get(i)->team)
        continue;
      if (!Player_get(j)->is_alive)
        continue;

      if (HitBox_collision(&Proj_get(i)->hitbox, &Player_get(j)->hitbox)) {
        Player_deal_damage(Player_get(j), 10.0f);
        Proj_get(i)->is_checked_by_server = 0;
        Proj_get(i)->is_alive = 0;
      }
    }
  }

  Player_update_all(dt);

  for (int i = 0; i < Player_num_players(); i++) {
    if (Player_get(i) && Player_get(i)->health <= 0) {
      Player_get(i)->is_alive = 0;
    }
  }
}
