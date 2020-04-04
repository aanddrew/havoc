#include <SDL2/SDL.h>
#include <string.h>

#include "graphics/Window.h"
#include "graphics/Dolly.h"
#include "graphics/Camera.h"

#include "engine/Player.h"
#include "engine/Controller.h"
#include "engine/Projectile.h"

#include "network/Client_Pool.h"
#include "network/Client_Sender.h"
#include "network/Client_Receiver.h"

#ifdef WIN32
    #define strdup _strdup
#endif

int main(int argc, char** argv)
{
    Window* window = Window_init();
    Proj_init_all_sprites(window->renderer);

    /* Initialize Network */

    Pool_init();
    Client_Receiver_init(21433);
    Client_Sender_init(21432);
    Client_Sender_connect("127.0.0.1");

    Client_Receiver_run();
    Client_Sender_run();
 
    /* Initialize Game */

    Dolly wiz;
    Dolly_init_with_sprites(&wiz, window->renderer, "res/wizard/wizard_16_00.bmp", 9);

    Player p;

    Player_init(&p);
    p.sprite = &wiz;

    Camera cam;
    Camera_init(&cam);

    {
        SDL_Rect viewport;
        SDL_RenderGetViewport(window->renderer, &viewport);

        Camera_set_size(&cam, viewport.w, viewport.h);
        Camera_set_center(&cam, p.pos.x, p.pos.y);
    }

    Controller c;
    Controller_init(&c);
    c.player = &p;
    c.cam = &cam;

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
                case SDL_MOUSEBUTTONDOWN:
                    Controller_mousebuttondown(&c, e.button);
                break;
                case SDL_MOUSEWHEEL: 
                    Controller_mousewheel(&c, e.wheel);
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

        float dt_float = ((float) dt) / 1000.0f;
        Controller_update(&c, dt_float, &cam);
        Proj_update_all(dt_float);

        Window_clear(window);
		
        Dolly_render(&wiz, window->renderer, &cam);
        Proj_render_all(window->renderer, &cam);

        Window_present(window);
	}

    Dolly_delete(&wiz);
    Proj_cleanup_all_sprites();
    Window_delete(window);

    SDL_LockMutex(shared_pool.running_mutex);
        shared_pool.running = 0;
    SDL_UnlockMutex(shared_pool.running_mutex);

    Client_Receiver_stop();
    Client_Sender_stop();

    Client_Receiver_deinit();
    Client_Sender_deinit();
	return 0;
}
