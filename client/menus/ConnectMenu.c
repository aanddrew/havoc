#include "ConnectMenu.h"
#include "../../gui/Button.h"
#include "../../gui/TextBox.h"

#include "../../SDL_FontCache/SDL_FontCache.h"

TextBox ip_box;
TextBox name_box;
SDL_Renderer* my_renderer;

static const char* button_text[] = {
    "back",
    "join"
};

static Button buttons[CONNECTMENU_NUM_BUTTONS];

void ConnectMenu_init(SDL_Renderer* renderer)
{
    my_renderer = renderer;

    TextBox_init(&ip_box, "type ip", 20);
    ip_box.is_active = 0;
    ip_box.box_width = 300;

    TextBox_init(&name_box, "type name", 20);
    name_box.is_active = 0;
    name_box.box_width = 300;
    SDL_StartTextInput();

    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    ip_box.x = w / 2 - 150;
    ip_box.y = h / 2 - 40;

    name_box.x = ip_box.x;
    name_box.y = ip_box.y - 80;

    for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
        Button_init_text(&buttons[i], button_text[i], 16);
        buttons[i].rect.x = (w / 2) - 90 + 90 * i;
        buttons[i].rect.y = h / 2 + 50;
    }
}

void ConnectMenu_deinit()
{
    for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
        Button_deinit(&buttons[i]);
    }
    TextBox_deinit(&ip_box);
    SDL_StopTextInput();
}

static TextBox* current_box;

void ConnectMenu_event(SDL_Event e)
{
    switch (e.type) {
    case SDL_KEYDOWN: {
        switch (e.key.keysym.sym) {
        case SDLK_BACKSPACE:
            if (current_box)
                TextBox_delete_end(current_box);
            break;
        }
    } break;
    case SDL_TEXTINPUT:
        if (current_box)
            TextBox_append(current_box, e.text.text);
        break;
    case SDL_MOUSEBUTTONDOWN: {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (TextBox_is_mouse_inside(&ip_box, my_renderer, x, y)) {
            name_box.is_active = 0;
            ip_box.is_active = 1;
            current_box = &ip_box;
        } else if (TextBox_is_mouse_inside(&name_box, my_renderer, x, y)) {
            name_box.is_active = 1;
            ip_box.is_active = 0;
            current_box = &name_box;
        } else {
            name_box.is_active = 0;
            ip_box.is_active = 0;
            current_box = NULL;
        }
    } break;
    }
}

void ConnectMenu_render(SDL_Renderer* renderer)
{
    TextBox_render_bg(&ip_box, renderer);
    TextBox_render_bg(&name_box, renderer);

    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
        buttons[i].is_hovered = Button_is_mouse_inside(&buttons[i], renderer, mousex, mousey);
        Button_render(&buttons[i], renderer);
    }
}

void ConnectMenu_getip(char* buf, int len)
{
    if ((int)strlen(ip_box.buffer) >= len)
        return;
    for (int i = 0; i < (int)strlen(ip_box.buffer) + 1; i++) {
        buf[i] = ip_box.buffer[i];
    }
}

void ConnectMenu_getname(char* buf, int len)
{
    if ((int)strlen(name_box.buffer) >= len)
        return;
    for (int i = 0; i < (int)strlen(name_box.buffer) + 1; i++) {
        buf[i] = name_box.buffer[i];
    }
}

int ConnectMenu_pressed_button(SDL_Renderer* renderer, int x, int y)
{
    for (int i = 0; i < CONNECTMENU_NUM_BUTTONS; i++) {
        if (Button_is_mouse_inside(&buttons[i], renderer, x, y)) {
            return i;
        }
    }
    return -1;
}
