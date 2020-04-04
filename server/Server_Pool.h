#ifndef SERVER_POOL_H
#define SERVER_POOL_H

#include "../utils/Vector.h"

#include <SDL2/SDL.h>

typedef struct {
    Vector* received;
    Vector* sending;
    int running;

    SDL_mutex* received_mutex;
    SDL_mutex* sending_mutex;
    SDL_mutex* running_mutex;
} pool_t;

extern pool_t shared_pool;

void Pool_init();
void Pool_deinit();

#endif
