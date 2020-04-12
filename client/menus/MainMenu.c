#include "MainMenu.h"
#include "../../gui/Button.h"

#include "../../SDL_FontCache/SDL_FontCache.h"
#include <SDL2/SDL_ttf.h>

const char* button_text[] = {
    "connect",
    "options",
    "quit"
};

static Button buttons[NUM_BUTTONS];
static Button havoc_button;

static SDL_Texture* picture_texture;
static SDL_Rect picture_rect;

void MainMenu_init(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_LoadBMP("res/pictures/wizards_standing_pixel.bmp");
    picture_texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!surface || !picture_texture) {
        printf("Error opening main menu picture %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);

    picture_rect.x = 300;
    picture_rect.y = 100;
    picture_rect.w = 800;
    picture_rect.h = 400;

    int start_y = 200;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        Button_init_text(&buttons[i], button_text[i], 16);
        buttons[i].rect.x = 50;
        buttons[i].rect.y = start_y;
        start_y += buttons[i].rect.h + 10;
    }
    Button_init_text(&havoc_button, "HAVOC", 24);
    havoc_button.rect.x = 50;
    havoc_button.rect.y = 100;
}

void MainMenu_deinit()
{
    SDL_DestroyTexture(picture_texture);
    picture_texture = NULL;
}

SDL_Texture* message_texture = NULL;
void MainMenu_Render(SDL_Renderer* renderer)
{
    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].is_hovered = Button_is_mouse_inside(&buttons[i], renderer, mousex, mousey);
        Button_render(&buttons[i], renderer);
    }
    Button_render(&havoc_button, renderer);

    SDL_RenderCopy(renderer, picture_texture, NULL, &picture_rect);
}

int MainMenu_pressed_button(SDL_Renderer* renderer, int x, int y)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (Button_is_mouse_inside(&buttons[i], renderer, x, y)) {
            return i;
        }
    }
    return -1;
}
