#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../SDL_FontCache/SDL_FontCache.h"

#include "../gui/Gui.h"

#include "../renderers/GameRenderer.h"
#include "../renderers/MapRenderer.h"

#include "../game/Game.h"

#include "Controller.h"
#include "GameLoop.h"

#include "network/Client_Pool.h"
#include "network/Client_Receiver.h"
#include "network/Client_Sender.h"
#include "network/Network.h"

#include "../utils/Network_utils.h"

#include "States.h"

#ifdef WIN32
#define strdup _strdup
#endif

char server_hostname[512];
char wish_name[64];

int MainMenu_Loop(Window *window);
int ConnectMenu_Loop(Window *window);

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Error, not supposed to take arguments\n");
    for (int i = 1; i < argc; i++) {
      printf("%s\n", argv[i]);
    }
  }
  Window *window = Window_init();
  Fonts_init(window->renderer);
  int current_state = MAIN_MENU;

  while (current_state != EXIT_PROGRAM) {
    switch (current_state) {
    case MAIN_MENU:
      current_state = MainMenu_Loop(window);
      break;
    case CONNECT:
      current_state = ConnectMenu_Loop(window);
      break;
    case OPTIONS:
      current_state = MainMenu_Loop(window);
      break;
    case GAME:
      current_state = Game_Loop(window, server_hostname, wish_name);
      break;
    }
  }

  Fonts_deinit();
  Window_delete(window);
}

enum MAINMENU_ITEMS {
  MAINMENU_PICTURE_BTN,
  MAINMENU_HAVOC_BTN,

  MAINMENU_CONNECT_BTN,
  MAINMENU_OPTIONS_BTN,
  MAINMENU_QUIT_BTN,

  MAINMENU_NUM_BUTTONS,
};

int MainMenu_Loop(Window *window) {
  /* Initialize Main Menu */
  Menu main_menu;
  Menu_init(&main_menu);

  // ids to reference the menu items by
  int ids[MAINMENU_NUM_BUTTONS];

  // add picture of wizards to the main menu
  {
    Button but;
    Button_init_icon(&but, window->renderer,
                     "res/pictures/wizards_standing_pixel.bmp");

    but.rect.x = 300;
    but.rect.y = 100;
    but.rect.w = 800;
    but.rect.h = 400;

    but.srcrect.w = 800;
    but.srcrect.h = 400;

    but.is_dummy = 1;
    ids[MAINMENU_PICTURE_BTN] = Menu_add_button(&main_menu, but);
  }

  // add larger havoc button to the main menu
  {
    Button but;
    Button_init_text(&but, "HAVOC", 36);
    but.rect.x = 50;
    but.rect.y = 140;
    but.is_dummy = 1;
    ids[MAINMENU_HAVOC_BTN] = Menu_add_button(&main_menu, but);
  }

  // add functional buttons
  {
    const char *button_text[] = {"connect", "options", "quit"};

    int start_y = 200;
    for (int i = 0; i < MAINMENU_QUIT_BTN - MAINMENU_CONNECT_BTN + 1; i++) {
      Button but;
      Button_init_text(&but, button_text[i], 20);
      but.rect.x = 50;
      but.rect.y = start_y;
      start_y += but.rect.h + 10;
      ids[MAINMENU_CONNECT_BTN + i] = Menu_add_button(&main_menu, but);
    }
  }

  /* Now we begin polling events */

  SDL_Event e;
  int done = 0;
  int ret = EXIT_PROGRAM;
  while (!done) {
    while (SDL_PollEvent(&e)) {
      Menu_pass_event(&main_menu, window->renderer, &e);
      switch (e.type) {
      case SDL_KEYDOWN: {
        if (e.key.keysym.sym == SDLK_c) {
          done = 1;
          ret = CONNECT;
        }
      } break;
      case SDL_MOUSEBUTTONDOWN: {
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);
        int button = main_menu.selected_button;
        if (button == ids[MAINMENU_QUIT_BTN]) {
          done = 1;
          ret = EXIT_PROGRAM;
        } else if (button == ids[MAINMENU_CONNECT_BTN]) {
          done = 1;
          ret = CONNECT;
        } else if (button == ids[MAINMENU_OPTIONS_BTN]) {
          done = 1;
          ret = OPTIONS;
        }
      } break;
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {

        } break;
        }
        break;
      case SDL_QUIT:
        done = 1;
        ret = EXIT_PROGRAM;
        break;
      }
    }
    Window_clear(window);
    Menu_render(&main_menu, window->renderer);
    Window_present(window);
  }
  Menu_deinit(&main_menu);

  return ret;
}

enum CONNECTMENU_ITEMS {
  CONNECTMENU_IP_BOX,
  CONNECTMENU_NAME_BOX,
  CONNECTMENU_BACK_BTN,
  CONNECTMENU_JOIN_BTN,
  CONNECTMENU_NUM_ITEMS,
};

int ConnectMenu_Loop(Window *window) {
  Menu connect_menu;
  Menu_init(&connect_menu);

  int ids[CONNECTMENU_NUM_ITEMS];
  {
    // textboxes
    TextBox ip_box, name_box;

    TextBox_init(&ip_box, "type ip", 20);
    ip_box.centerx = 1;
    ip_box.centery = 1;
    ip_box.x = -150;
    ip_box.y = 0;
    ip_box.box_width = 300;

    TextBox_init(&name_box, "type name", 20);
    name_box.centerx = 1;
    name_box.centery = 1;
    name_box.x = -150;
    name_box.y = -50;
    name_box.box_width = 300;

    ids[CONNECTMENU_NAME_BOX] = Menu_add_textbox(&connect_menu, name_box);
    ids[CONNECTMENU_IP_BOX] = Menu_add_textbox(&connect_menu, ip_box);

    // back and join buttons
    Button back_button, join_button;
    Button_init_text(&back_button, "back", 20);
    back_button.centerx = 1;
    back_button.centery = 1;
    back_button.rect.x = -80;
    back_button.rect.y = 50;

    Button_init_text(&join_button, "join", 20);
    join_button.centerx = 1;
    join_button.centery = 1;
    join_button.rect.x = 30;
    join_button.rect.y = 50;

    ids[CONNECTMENU_JOIN_BTN] = Menu_add_button(&connect_menu, join_button);
    ids[CONNECTMENU_BACK_BTN] = Menu_add_button(&connect_menu, back_button);
  }

  SDL_Event e;
  int done = 0;
  int ret = EXIT_PROGRAM;
  while (!done) {
    while (SDL_PollEvent(&e)) {
      Menu_pass_event(&connect_menu, window->renderer, &e);
      switch (e.type) {
      case SDL_QUIT:
        done = 1;
        ret = EXIT_PROGRAM;
        break;
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          done = 1;
          ret = MAIN_MENU;
        }
        if (e.key.keysym.sym == SDLK_RETURN) {
          TextBox *ip_box =
              Menu_get_textbox(&connect_menu, ids[CONNECTMENU_IP_BOX]);
          TextBox *name_box =
              Menu_get_textbox(&connect_menu, ids[CONNECTMENU_NAME_BOX]);

          strncpy(server_hostname, ip_box->buffer, 512);
          strncpy(wish_name, name_box->buffer, 64);
          server_hostname[511] = '\0';
          wish_name[63] = '\0';

          done = 1;
          ret = GAME;
        }
        break;
      case SDL_MOUSEBUTTONDOWN: {
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);
        int button_pressed = connect_menu.selected_button;
        if (button_pressed == ids[CONNECTMENU_JOIN_BTN]) {
          TextBox *ip_box =
              Menu_get_textbox(&connect_menu, ids[CONNECTMENU_IP_BOX]);
          TextBox *name_box =
              Menu_get_textbox(&connect_menu, ids[CONNECTMENU_NAME_BOX]);

          strncpy(server_hostname, ip_box->buffer, 512);
          strncpy(wish_name, name_box->buffer, 64);
          server_hostname[511] = '\0';
          wish_name[63] = '\0';

          done = 1;
          ret = GAME;
        } else if (button_pressed == ids[CONNECTMENU_BACK_BTN]) {
          done = 1;
          ret = MAIN_MENU;
        }
      } break;
      }
    }
    Window_clear(window);
    Menu_render(&connect_menu, window->renderer);
    Window_present(window);
  }

  Menu_deinit(&connect_menu);
  return ret;
}
