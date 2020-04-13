#include "Menu.h"

//only one textbox can be active across all menus
TextBox* current_textbox = NULL;

void Menu_init(Menu* self)
{
    Vector_init(&self->buttons);
    Vector_init(&self->textboxes);
    self->is_hidden = 0;
    self->selected_button = -1;
    self->selected_box = -1;
}

void Menu_deinit(Menu* self)
{
    Vector_delete(&self->buttons);
    Vector_delete(&self->textboxes);
}

int Menu_add_button(Menu* self, Button b)
{
    Button* newbutton = malloc(sizeof(Button));
    *newbutton = b;
    Vector_push(&self->buttons, newbutton);
    return self->buttons.num - 1;
}

int Menu_add_textbox(Menu* self, TextBox t)
{
    TextBox* newbox = malloc(sizeof(TextBox));
    *newbox = t;
    Vector_push(&self->textboxes, newbox);
    return self->textboxes.num - 1;
}

Button* Menu_get_button(Menu* self, int id)
{
    return Vector_get(&self->buttons, id);
}

TextBox* Menu_get_textbox(Menu* self, int id)
{
    return Vector_get(&self->textboxes, id);
}

void Menu_pass_event(Menu* self, SDL_Renderer* renderer, const SDL_Event* e)
{
    switch (e->type) {
    case SDL_KEYDOWN:
        if (!current_textbox) {
        } else {
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                current_textbox->is_active = 0;
                self->selected_box = -1;
                current_textbox = NULL;
            } else if (e->key.keysym.sym == SDLK_BACKSPACE) {
                TextBox_delete_end(current_textbox);
            } else if (e->key.keysym.sym == SDLK_TAB) {
                current_textbox->is_active = 0;
                self->selected_box = (self->selected_box + 1) % self->textboxes.num;
                current_textbox = Vector_get(&self->textboxes, self->selected_box);
                current_textbox->is_active = 1;
            }
        }
        break;
    case SDL_TEXTINPUT:
        if (current_textbox) {
            TextBox_append(current_textbox, e->text.text);
        }
        break;
    case SDL_MOUSEBUTTONDOWN: {
        if (e->button.button == SDL_BUTTON_LEFT) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            int in_textbox = 0;
            for (int i = 0; i < self->textboxes.num; i++) {
                TextBox* box = Vector_get(&self->textboxes, i);
                if (TextBox_is_mouse_inside(box, renderer, x, y)) {
                    if (current_textbox && box != current_textbox) {
                        current_textbox->is_active = 0;
                    }

                    current_textbox = box;
                    current_textbox->is_active = 1;

                    self->selected_box = i;
                    in_textbox = 1;

                    TextBox_reset_cursor_blink_time();
                    SDL_StartTextInput();
                    break;
                }
            }
            if (!in_textbox && current_textbox) {
                current_textbox->is_active = 0;
                current_textbox = NULL;
            }
            for (int i = 0; i < self->buttons.num; i++) {
                Button* but = Vector_get(&self->buttons, i);
                if (but->is_hidden || but->is_dummy)
                    continue;
                if (Button_is_mouse_inside(but, renderer, x, y)) {
                    self->selected_button = i;
                    break;
                }
            }
        }
    } break;
    }
}

const TextBox* Menu_get_selected_textbox()
{
    return current_textbox;
}

void Menu_deselect_textbox()
{
    if (!current_textbox)
        return;

    current_textbox->is_active = 0;
    current_textbox = NULL;
}

void Menu_render(Menu* self, SDL_Renderer* renderer)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (self->is_hidden) {
        return;
    }

    for (int i = 0; i < self->buttons.num; i++) {
        Button* but = Vector_get(&self->buttons, i);
        if (!but->is_dummy) {
            but->is_hovered = Button_is_mouse_inside(but, renderer, x, y);
        }
        Button_render(but, renderer);
    }

    for (int i = 0; i < self->textboxes.num; i++) {
        TextBox_render_bg(Vector_get(&self->textboxes, i), renderer);
    }
}
