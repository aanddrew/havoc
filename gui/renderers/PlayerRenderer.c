#include "PlayerRenderer.h"

static Dolly wizard;

void Player_init_all_sprites(SDL_Renderer* renderer) {
    Dolly_init_with_sprites(&wizard, renderer, "res/wizard/wizard_16_00.bmp", 9);
}

void Player_cleanup_all_sprites() {
    Dolly_delete(&wizard);
}

static void Player_update_sprite(Player* self) {

    static const float LOOK_OFFSET = -3.0 * M_PI / 4.0;

    wizard.rect.x = (int) (self->pos.x) - wizard.rect.w/2;
    wizard.rect.y = (int) (self->pos.y) - wizard.rect.h/2;
    float angle = Vector2d_angle(self->look);
    wizard.angle = angle + LOOK_OFFSET;
}

void Player_render_all(SDL_Renderer* renderer, const Camera* cam) {
    for(int i = 0; i < Player_num_players(); i++) {
        Player* p = Player_get(i);
        if (p) {
            Player_update_sprite(Player_get(i));
            Dolly_render(&wizard, renderer, cam);
        }
    }
}
