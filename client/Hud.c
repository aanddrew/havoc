#include "Hud.h"
#include "../gui/Atlas.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "../gui/Gui.h"

enum HUD_ITEMS {
  HUD_HEALTH_BTN,
  HUD_INVENTORY_BG,
  HUD_NUM_ITEMS,
};

static int ids[HUD_NUM_ITEMS];

static Atlas item_atlas;

static Menu hud_menu;

void Hud_init(SDL_Renderer *renderer) {
  Menu_init(&hud_menu);

  Atlas_init_texture(&item_atlas, renderer, "res/items/items.png", 16);

  Button inventory_bg;
  Button_init(&inventory_bg);
  inventory_bg.draw_background = 1;
  inventory_bg.rect.x = -150;
  inventory_bg.rect.y = -40;
  inventory_bg.rect.w = 300;
  inventory_bg.rect.h = 40;
  inventory_bg.centerx = 1;
  inventory_bg.is_dummy = 1;
  ids[HUD_INVENTORY_BG] = Menu_add_button(&hud_menu, inventory_bg);

  Button health_but;
  Button_init_text(&health_but, "health placeholder", 32);
  health_but.rect.x = -250;
  health_but.rect.y = -50;
  health_but.centerx = 1;
  health_but.is_dummy = 1;
  ids[HUD_HEALTH_BTN] = Menu_add_button(&hud_menu, health_but);
}

void Hud_deinit() {}

char health_buf[128];
void Hud_render(SDL_Renderer *renderer, Player *p) {
  sprintf(health_buf, "%.1f", p->health);
  Menu_get_button(&hud_menu, ids[HUD_HEALTH_BTN])->text = health_buf;
  Menu_render(&hud_menu, renderer);
}
