#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../graphics/Window.h"
#include "../graphics/Dolly.h"
#include "../graphics/Camera.h"

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

int main(int argc, char** argv)
{
    Window* window = Window_init();

    Camera cam;
    Camera_init(&cam, window->renderer);

    Map map;
    Map_init(&map, window->renderer, NULL);

    int current_time = SDL_GetTicks();
    int dt = 0;
    SDL_Event e;
    int done = 0;
    while (!done)
    {
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();
        float float_dt = ((float) dt) / 1000.0f;

        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                for(int i = 0; i < NUM_BINDS; i++) {
                    if (e.key.keysym.sym == BINDS[i]) {
                        bind_pressed[i] = 1;
                    }
                } break;
            case SDL_KEYUP:
                for(int i = 0; i < NUM_BINDS; i++) {
                    if (e.key.keysym.sym == BINDS[i]) {
                        bind_pressed[i] = 0;
                    }
                } break;
            case SDL_MOUSEBUTTONDOWN: {
                if(e.button.button == SDL_BUTTON_LEFT) {
                    mouse_button_pressed[LEFT_BUTTON] = 1;
                }

                int x,y;
                SDL_GetMouseState(&x, &y);

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

        if (mouse_button_pressed[LEFT_BUTTON]) {
            int worldx, worldy;
            Camera_get_mousestate_relative(&cam, &worldx, &worldy);

            Map_set_tile(&map, 0, (int) worldx / 16, (int) worldy / 16);
        }

        Window_clear(window);

        Map_render(&map, window->renderer, &cam);

        Window_present(window);
    }
   
    Window_delete(window);
}
