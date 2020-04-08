#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../graphics/Window.h"
#include "../graphics/Dolly.h"
#include "../graphics/Camera.h"
#include "../graphics/Button.h"
#include "../graphics/TextBox.h"

#include "Map.h"

#ifdef WIN32
    #define strdup _strdup
#endif

enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NUM_BINDS,
};

static const int BINDS[] = {
    SDLK_w,
    SDLK_s,
    SDLK_a,
    SDLK_d,
};
static int bind_pressed[NUM_BINDS];
static int camera_speed = 200.0f;

enum mouse_binds {
    LEFT_BUTTON,
    RIGHT_BUTTON,
    MIDDLE_BUTTON,
    NUM_BUTTONS,
};
static int mouse_button_pressed[NUM_BUTTONS];

static Button tile_buttons[NUM_TILES];

static TextBox save_box;
FC_Font* font = NULL;

int main(int argc, char** argv)
{
    Window* window = Window_init();

    Camera cam;
    Camera_init(&cam, window->renderer);

    Map map;
    Map_init(&map, window->renderer, NULL);

    for(int i = 0; i < NUM_TILES; i++) {
        Button_init_icon(&tile_buttons[i], window->renderer, tile_files[i]);
        tile_buttons[i].rect.y = i * tile_buttons[i].rect.h;
    }

    font = FC_CreateFont();
    FC_LoadFont(font, window->renderer, "../res/fonts/5ceta_mono.ttf", 10, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

    TextBox_init(&save_box, "output file name", font);
    save_box.x = 25;
    save_box.y = -30;
    save_box.box_width = 200;

    TextBox* current_textbox = NULL;

    int selected_tile = 0;
    int screen_button_pressed = 0;

    int current_time = SDL_GetTicks();
    int dt = 0;
    SDL_Event e;
    int done = 0;
    while (!done)
    {
        screen_button_pressed = 0;
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();
        float float_dt = ((float) dt) / 1000.0f;

        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                if (!current_textbox) {
                    for(int i = 0; i < NUM_BINDS; i++) {
                        if (e.key.keysym.sym == BINDS[i]) {
                            bind_pressed[i] = 1;
                        }
                    } 
                }
                else {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        current_textbox->is_active = 0;
                        current_textbox = NULL;
                    }
                    if (e.key.keysym.sym == SDLK_BACKSPACE) {
                        TextBox_delete_end(current_textbox);
                    }
                }
                break;
            case SDL_KEYUP:
                if (!current_textbox) {
                    for(int i = 0; i < NUM_BINDS; i++) {
                        if (e.key.keysym.sym == BINDS[i]) {
                            bind_pressed[i] = 0;
                        }
                    } 
                }
                break;
            case SDL_TEXTEDITING:
                if (current_textbox) {
                    printf("%s\n", e.edit.text);
                }
                break;
            case SDL_TEXTINPUT:
                if (current_textbox) {
                    printf("%s\n", e.text.text);
                    TextBox_append(current_textbox, e.text.text);
                }
                break;
            case SDL_MOUSEBUTTONDOWN: {
                if(e.button.button == SDL_BUTTON_LEFT) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (current_textbox == NULL) {
                        if (TextBox_is_mouse_inside(&save_box,window->renderer, x, y)) {
                            current_textbox = &save_box;
                            current_textbox->is_active = 1;
                            for(int i = 0; i < NUM_BINDS; i++) {
                                bind_pressed[i] = 0;
                            }
                            SDL_StartTextInput();
                        }
                        else {
                            mouse_button_pressed[LEFT_BUTTON] = 1;
                        }
                    }
                    else {
                        if (!TextBox_is_mouse_inside(&save_box, window->renderer, x, y)) {
                            current_textbox->is_active= 0;
                            current_textbox = NULL;
                            mouse_button_pressed[LEFT_BUTTON] = 1;
                            SDL_StopTextInput();
                        }
                    }
   
                }
            } break;
            case SDL_MOUSEBUTTONUP:
                if(e.button.button == SDL_BUTTON_LEFT) {
                    mouse_button_pressed[LEFT_BUTTON] = 0;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (e.wheel.y > 0)
                    Camera_zoom(&cam, 1.1f);
                else if (e.wheel.y < 0)
                    Camera_zoom(&cam, 0.9f);
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    SDL_Rect viewport;
                    SDL_RenderGetViewport(window->renderer, &viewport);

                    Camera_set_size(&cam, viewport.w, viewport.h);
                } break;
                } break;
            case SDL_QUIT:
                done = 1;
                break;
            }
        }

        float move_amnt = camera_speed * float_dt;
        if (bind_pressed[UP])    { Camera_translate(&cam, 0, -move_amnt); }
        if (bind_pressed[DOWN])  { Camera_translate(&cam, 0, move_amnt);  }
        if (bind_pressed[LEFT])  { Camera_translate(&cam, -move_amnt, 0); }
        if (bind_pressed[RIGHT]) { Camera_translate(&cam, move_amnt, 0);  }

        if (mouse_button_pressed[LEFT_BUTTON] && !screen_button_pressed) {
            int x,y;
            SDL_GetMouseState(&x, &y);
            int screen_button_pressed = 0;
            for(int i = 0; i < NUM_TILES; i++) {
                if (Button_is_mouse_inside(&tile_buttons[i], x, y)) {
                    selected_tile = i;
                    screen_button_pressed = 1;
                }
            }
            if (!screen_button_pressed) {
                int worldx, worldy;
                Camera_get_mousestate_relative(&cam, &worldx, &worldy);
                worldx /= 16;
                worldy /= 16;
                for(int brush_x = worldx - 1; brush_x <= worldx + 1; brush_x++) {
                    for(int brush_y = worldy -1; brush_y <= worldy + 1; brush_y++) {
                        Map_set_tile(&map, selected_tile, brush_x, brush_y);
                    }
                }
            }
        }

        Window_clear(window);

        Map_render(&map, window->renderer, &cam);
        for(int i = 0; i < NUM_TILES; i++) {
            Button_render(&tile_buttons[i], window->renderer);
        }
        TextBox_render_bg(&save_box, window->renderer);

        Window_present(window);
    }
   
    Window_delete(window);
}
