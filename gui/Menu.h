#ifndef MENU_H
#define MENU_H

#include "../utils/Vector.h"
#include "Button.h"
#include "TextBox.h"

typedef struct {
    Vector buttons;
    Vector textboxes;

    int selected_button;
    int selected_box;

    int is_hidden;
} Menu;

void Menu_init(Menu* menu);
void Menu_deinit(Menu* menu);

int Menu_add_button(Menu* menu, Button b);
int Menu_add_textbox(Menu* menu, TextBox t);

Button* Menu_get_button(Menu* menu, int id);
TextBox* Menu_get_textbox(Menu* menu, int id);

void Menu_pass_event(Menu* self, SDL_Renderer* renderer, const SDL_Event* e);

const TextBox* Menu_get_selected_textbox();
void Menu_deselect_textbox();

void Menu_render(Menu* self, SDL_Renderer* renderer);

#endif
