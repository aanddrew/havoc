#ifndef MENU_H
#define MENU_H

#include "../utils/Vector.h"
#include "Button.h"
#include "TextBox.h"

typedef struct {
  Vector buttons;
  Vector textboxes;

  int selected_button; // the button that was clicked last
  int selected_box;    // the textbox which was clicked last

  int is_hidden;
} Menu;

void Menu_init(Menu *menu);
void Menu_deinit(Menu *menu);

// returns an id to the internal vectors of buttons/textboxes
int Menu_add_button(Menu *menu, Button b);
int Menu_add_textbox(Menu *menu, TextBox t);

// accesses the items from the ids of the methods above
Button *Menu_get_button(Menu *menu, int id);
TextBox *Menu_get_textbox(Menu *menu, int id);

// clicks buttons, fills textboxes, etc... sets selected_button and box
void Menu_pass_event(Menu *self, SDL_Renderer *renderer, const SDL_Event *e);

// this is a global textbox across all menus, as only 1 textbox should be active
// at a time
const TextBox *Menu_get_selected_textbox();
void Menu_deselect_textbox();

void Menu_render(Menu *self, SDL_Renderer *renderer);

#endif
