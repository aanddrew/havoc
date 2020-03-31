#include <SDL2/SDL.h>
#include <string.h>

#include "Dolly.h"
#include "Window.h"

#ifdef WIN32
    #define strdup _strdup
#endif

int main(int argc, char** argv)
{
    Window* window = Window_init();

    Dolly* wiz = Dolly_init();
    Dolly_setSprites(wiz, window->renderer, "res/wizard/wizard_16_00.bmp", 9);

	SDL_Event e;
	int done = 0;
	while(!done)
	{
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					done = 1;
				break;
			}
		}
        Window_clear(window);
		
        Dolly_render(wiz, window->renderer);
        Dolly_translate(wiz, 1, 1);
        Dolly_rotate(wiz, 2);

        Window_present(window);
	}

    Dolly_delete(wiz);
    Window_delete(window);
	return 0;
}
