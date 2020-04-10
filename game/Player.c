#include "Player.h"
#include "Vector2d.h"

#include "../global.h"

static Player players[16];
static short num_players = 0;
static short size_players = 16;

void Player_init(Player* self) {
    self->pos.x = 0.0f;
    self->pos.y = 0.0f;
    self->speed = 200.0f;

    self->name = NULL;

    self->is_alive = 0;
    self->is_connected = 0;
}

void Player_init_all() {
    for(int i = 0; i < size_players; i++) {
        Player_init(&players[i]);
    }
}

void Player_update(Player* self, float dt) {
    Player_translate(self, self->vel.x * dt, self->vel.y * dt);
}

void Player_translate(Player* self, float dx, float dy) {
    self->pos.x += dx;
    self->pos.y += dy;
}

void Player_update_all(float dt) {
    for(int i = 0; i < size_players; i++) {
        Player *p = Player_get(i);
        if (p && p->is_connected) {
            Player_update(p, dt);
        }
    }
}

Player* Player_connect_with_id(char* name, int id) {
    if (id >= size_players || id < 0) {
        return NULL;
    }

    if (players[id].is_connected) {
        printf("Error connecting player \'%s\': id %d is already taken!!\n", name, id);
        return NULL;
    }

    Player_init(&players[id]);
    players[id].name = strdup(name);
    players[id].is_connected = 1;
    return &players[id];
}

Player* Player_connect(char* name, int* id) {
    int temp_id = -1;
    for(int i = 0; i < size_players; i++) {
        if (!players[i].is_connected) {
            temp_id = i;
            break;
        }
    }

    if (id != NULL) {
        *id = temp_id;
    }
    return Player_connect_with_id(name, temp_id);
}

int Player_disconnect(int id) {
    Player* player = Player_get(id);
    player->is_connected = 0;
    free(player->name);
}

void Player_set_name(char* name, int id) {
    Player* player = Player_get(id);
    if (!player) return;

    free(player->name);
    player->name = strdup(name);
}

int Player_num_players() {
   return 16;
}

Player* Player_get(int index) {
    if (index >= size_players || index < 0) {
        return NULL;
    }
    if (!players[index].is_connected) {
        return NULL;
    }
    return &players[index];
}

