#include <SDL2/SDL.h>
#include <string.h>

#include "graphics/Window.h"
#include "graphics/Dolly.h"

#include "engine/Player.h"
#include "engine/Controller.h"

#ifdef WIN32
    #define strdup _strdup
#endif

int main(int argc, char** argv)
{
    Window* window = Window_init();

    Dolly* wiz = Dolly_init();
    Dolly_setSprites(wiz, window->renderer, "res/wizard/wizard_16_00.bmp", 9);

    Player p;
    Player_init(&p);
    p.sprite = wiz;
    Controller c;
    Controller_init(&c);
    c.player = &p;

	SDL_Event e;
	int done = 0;
    int current_time = SDL_GetTicks();
    int dt = 0;
	while(!done)
	{
        dt = SDL_GetTicks() - current_time;
        current_time = SDL_GetTicks();

		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
                case SDL_KEYDOWN:
                    Controller_keydown(&c, e.key.keysym.sym);
                break;
                case SDL_KEYUP:
                    Controller_keyup(&c, e.key.keysym.sym);
                break;
				case SDL_QUIT:
					done = 1;
				break;
			}
		}
        Controller_update(&c, ((float) dt) / 1000.0f);
        Window_clear(window);
		
        Dolly_render(wiz, window->renderer);
        //Dolly_translate(wiz, 1, 1);
        //Dolly_rotate(wiz, 2);

        Window_present(window);
	}

    Dolly_delete(wiz);
    Window_delete(window);
	return 0;
}
