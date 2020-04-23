#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../gui/Button.h"
#include "../gui/Camera.h"
#include "../gui/Dolly.h"
#include "../gui/Fonts.h"
#include "../gui/Menu.h"
#include "../gui/TextBox.h"
#include "../gui/Window.h"

#include "../game/Map.h"
#include "../renderers/MapRenderer.h"

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

enum ITEMS {
    SAVE_BTN,
    LOAD_BTN,
    SAVE_BOX,
    TILE_BTN,
    SIZE_BOX,
    DEPTH_BOX,
    SPAWNER_BOX,
    NUM_ITEMS,
};

static int IDS[NUM_ITEMS];
#define MAX_NUM_TILES 64
static int TILE_IDS[64];
static int num_tiles = 0;
#define MAX_LAYERS 8
static int LAYER_IDS[MAX_LAYERS];

static int brush_size = 1;
static int brush_depth = 0;

static Menu top_menu;
void init_top_menu();

static Window* window;

int main(int argc, char** argv)
{
    if (argc != 1) {
        printf("Error, not expecting command line arguments\n");
        for (int i = 1; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    }
    window = Window_init();
    Fonts_init(window->renderer);

    Camera cam;
    Camera_init(&cam, window->renderer);

    MapRenderer_init(window->renderer);

    Map map;
    Map_init(&map, NULL);

    init_top_menu();
    int screen_button_pressed = 0;

    //these dictate what happens when left clicking on map
    enum SELECTION_TYPE {
        TILE,
        SPAWNER,
    };
    struct {
        int type;
        int num;
    } selection;

    selection.type = 0;
    selection.num = 0;

    int current_time = SDL_GetTicks();
    int dt = 0;
    SDL_Event e;
    int done = 0;
    while (!done) {
        screen_button_pressed = 0;
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();
        float float_dt = ((float)dt) / 1000.0f;

        while (SDL_PollEvent(&e)) {
            Menu_pass_event(&top_menu, window->renderer, &e);
            switch (e.type) {
            case SDL_KEYDOWN:
                for (int i = 0; i < NUM_BINDS; i++) {
                    if (e.key.keysym.sym == BINDS[i]) {
                        bind_pressed[i] = 1;
                    }
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        const TextBox* box = Menu_get_selected_textbox();
                        if (box) {
                            int num;
                            if (sscanf(box->buffer, "%d", &num)) {
                                if (top_menu.selected_box == IDS[SIZE_BOX]) {
                                    brush_size = num;
                                } else if (top_menu.selected_box == IDS[DEPTH_BOX]) {
                                    brush_depth = num;
                                } else if (top_menu.selected_box == IDS[SPAWNER_BOX]) {
                                    printf("setting spawner to %d\n", num);
                                    selection.type = SPAWNER;
                                    selection.num = num;
                                }
                            }
                            Menu_deselect_textbox();
                            top_menu.selected_box = -1;
                        }
                    }
                }
                break;
            case SDL_KEYUP:
                for (int i = 0; i < NUM_BINDS; i++) {
                    if (e.key.keysym.sym == BINDS[i]) {
                        bind_pressed[i] = 0;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN: {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_button_pressed[LEFT_BUTTON] = 1;
                }
            } break;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
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
                }
                break;
            case SDL_QUIT:
                done = 1;
                break;
            }
        }

        //if we're typing in textbox dont move camera around
        if (Menu_get_selected_textbox()) {
            for (int i = 0; i < NUM_BINDS; i++) {
                bind_pressed[i] = 0;
            }
        }

        //moving camera
        float move_amnt = camera_speed * float_dt;
        if (bind_pressed[UP]) {
            Camera_translate(&cam, 0, -move_amnt);
        }
        if (bind_pressed[DOWN]) {
            Camera_translate(&cam, 0, move_amnt);
        }
        if (bind_pressed[LEFT]) {
            Camera_translate(&cam, -move_amnt, 0);
        }
        if (bind_pressed[RIGHT]) {
            Camera_translate(&cam, move_amnt, 0);
        }

        screen_button_pressed = 0;
        //check for button presses in top menu
        if (top_menu.selected_button >= 0) {
            if (top_menu.selected_button == IDS[SAVE_BTN]) {
                Map_save(&map, Menu_get_textbox(&top_menu, IDS[SAVE_BOX])->buffer);
            } else if (top_menu.selected_button == IDS[LOAD_BTN]) {
                Map_load(&map, Menu_get_textbox(&top_menu, IDS[SAVE_BOX])->buffer);
            } else if (top_menu.selected_button == IDS[TILE_BTN]) {
                for (int i = 0; i < num_tiles; i++) {
                    Button* btn = Menu_get_button(&top_menu, TILE_IDS[i]);
                    btn->is_hidden = !btn->is_hidden;
                }
            }
            for (int i = 0; i < map.depth; i++) {
                if (top_menu.selected_button == LAYER_IDS[i]) {
                    Menu_get_button(&top_menu, top_menu.selected_button)->is_active = 1;
                    brush_depth = i;
                } else {
                    Menu_get_button(&top_menu, LAYER_IDS[i])->is_active = 0;
                }
            }
            for (int i = 0; i < num_tiles; i++) {
                if (top_menu.selected_button == TILE_IDS[i]) {
                    int atlas_w = MapRenderer_get_texture_width();

                    Button* tile_btn = Menu_get_button(&top_menu, IDS[TILE_BTN]);
                    tile_btn->srcrect.x = i % (atlas_w / 64) * 64;
                    tile_btn->srcrect.y = i / (atlas_w / 64) * 64;
                    selection.type = TILE;
                    selection.num = i;
                }
            }
            top_menu.selected_button = -1;
            screen_button_pressed = 1;
            for (int i = 0; i < NUM_BUTTONS; i++) {
                mouse_button_pressed[i] = 0;
            }
        }
        if (top_menu.selected_box >= 0) {
            screen_button_pressed = 1;
        }

        //brush painting, unless a menu button was pressed
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (mouse_button_pressed[LEFT_BUTTON] && !screen_button_pressed) {
            int worldx, worldy;
            Camera_get_mousestate_relative(&cam, &worldx, &worldy);
            worldx /= 64;
            worldy /= 64;
            switch (selection.type) {
            case SPAWNER: {
                printf("%d\n", selection.num);
                map.team_spawns[selection.num][0] = worldx;
                map.team_spawns[selection.num][1] = worldy;
            } break;
            case TILE: {
                int radius = brush_size - 1;
                for (int brush_x = worldx - radius; brush_x <= worldx + radius; brush_x++) {
                    for (int brush_y = worldy - radius; brush_y <= worldy + radius; brush_y++) {
                        Map_set_tile(&map, selection.num, brush_x, brush_y, brush_depth);
                    }
                }
            } break;
            }
        }

        //rendering
        Window_clear(window);
        Map_render(&map, window->renderer, &cam);
        Button* tile_button = Menu_get_button(&top_menu, TILE_IDS[0]);
        if (!tile_button->is_hidden) {
            SDL_Rect rect = tile_button->rect;
            rect.w = MapRenderer_get_texture_width();
            rect.h = MapRenderer_get_texture_height();
            SDL_SetRenderDrawColor(window->renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(window->renderer, &rect);
        }
        Menu_render(&top_menu, window->renderer);
        Window_present(window);
    }

    MapRenderer_deinit();
    Window_delete(window);
}

void init_top_menu()
{
    Menu_init(&top_menu);

    //save text box
    TextBox save_box;
    TextBox_init(&save_box, "output file name", 16);
    save_box.x = 25;
    save_box.y = -30;
    save_box.box_width = 200;

    //save and load buttons
    Button save_button;
    Button load_button;

    Button_init_text(&save_button, "save", 16);
    Button_init_text(&load_button, "load", 16);
    save_button.rect.x = 25;
    save_button.rect.y = -60;
    load_button.rect.x = 90;
    load_button.rect.y = -60;

    //tile button
    Button tile_button;
    Button_init_texture(&tile_button, MapRenderer_gettexture());
    tile_button.rect.x = 0;
    tile_button.rect.y = 0;
    tile_button.rect.w = 64;
    tile_button.rect.h = 64;
    tile_button.is_active = 1;

    //init tile selector/atlast buttons
    int atlas_w = MapRenderer_get_texture_width();
    int atlas_h = MapRenderer_get_texture_height();
    Button temp;
    int atlas_startx = 70;
    int atlas_starty = 20;

    int index = 0;
    for (int y = 0; y < atlas_h / 64; y++) {
        for (int x = 0; x < atlas_w / 64; x++) {
            if (index >= MAX_NUM_TILES) {
                printf("Error: too many tiles in texture!!!\n");
                break;
            }
            Button_init_texture(&temp, MapRenderer_gettexture());
            temp.rect.x = atlas_startx + x * 64;
            temp.rect.y = atlas_starty + y * 64;
            temp.rect.w = 64;
            temp.rect.h = 64;

            temp.srcrect.x = x * 64;
            temp.srcrect.y = y * 64;
            temp.srcrect.w = 64;
            temp.srcrect.h = 64;
            TILE_IDS[index] = Menu_add_button(&top_menu, temp);
            index++;
        }
    }
    num_tiles = index;

    //now do the cursor size box
    TextBox size_box;
    TextBox_init(&size_box, "cursor", 16);
    TextBox_append_char(&size_box, '1');
    size_box.x = 0;
    size_box.y = 96;
    size_box.box_width = 64;

    //cursor depth box
    TextBox depth_box;
    TextBox_init(&depth_box, "depth", 16);
    TextBox_append_char(&depth_box, '0');
    depth_box.x = 0;
    depth_box.y = 128;
    depth_box.box_width = 64;

    //layer selection boxes
    char layer_title[1024];
    int start_layer_x = -80;
    int start_layer_y = 192;
    for (int i = 0; i < MAX_LAYERS; i++) {
        Button layer;
        sprintf(layer_title, "layer %d", i);
        Button_init_text(&layer, layer_title, 16);
        layer.rect.x = start_layer_x;
        layer.rect.y = start_layer_y - i * 22;
        LAYER_IDS[i] = Menu_add_button(&top_menu, layer);
    }

    //team spawner box
    TextBox spawner_box;
    TextBox_init(&spawner_box, "team spawner", 16);
    TextBox_append_char(&spawner_box, '0');
    spawner_box.x = -80;
    spawner_box.y = 400;
    spawner_box.box_width = 50;
    IDS[SPAWNER_BOX] = Menu_add_textbox(&top_menu, spawner_box);

    IDS[DEPTH_BOX] = Menu_add_textbox(&top_menu, depth_box);
    IDS[SIZE_BOX] = Menu_add_textbox(&top_menu, size_box);

    IDS[SAVE_BTN] = Menu_add_button(&top_menu, save_button);
    IDS[LOAD_BTN] = Menu_add_button(&top_menu, load_button);
    IDS[SAVE_BOX] = Menu_add_textbox(&top_menu, save_box);
    IDS[TILE_BTN] = Menu_add_button(&top_menu, tile_button);
}
