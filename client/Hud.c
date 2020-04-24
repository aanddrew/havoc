#include "Hud.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "../gui/Gui.h"

enum HUD_ITEMS {
  HUD_HEALTH_BTN,
  HUD_NUM_ITEMS,
};

static Menu hud_menu;

void Hud_init() {
  Menu_init(&hud_menu);

  Button health_but;
  Button_init_text(&health_but, "health placeholder", 32);
  health_but.rect.x = -200;
  health_but.rect.y = -70;
  health_but.centerx = 1;
  health_but.is_dummy = 1;
  Menu_add_button(&hud_menu, health_but);
}

void Hud_deinit() {}

char health_buf[128];
void Hud_render(SDL_Renderer *renderer, Player *p) {
  sprintf(health_buf, "%.1f", p->health);
  Menu_get_button(&hud_menu, 0)->text = health_buf;
  Menu_render(&hud_menu, renderer);
}
