#include "PlayerRenderer.h"

#include "../gui/Fonts.h"

static Dolly wizard;

void Player_init_all_sprites(SDL_Renderer* renderer)
{
    Dolly_init_with_sprites(&wizard, renderer, "res/wizard/wizard_16_00.bmp", 9);
}

void Player_cleanup_all_sprites()
{
    Dolly_delete(&wizard);
}

static void Player_update_sprite(Player* self)
{

    static const float LOOK_OFFSET = -3.0 * M_PI / 4.0;

    wizard.rect.x = (int)(self->pos.x) - wizard.rect.w / 2;
    wizard.rect.y = (int)(self->pos.y) - wizard.rect.h / 2;
    float angle = Vector2d_angle(self->look);
    wizard.angle = angle + LOOK_OFFSET;
}

void Player_render_all(SDL_Renderer* renderer, const Camera* cam)
{
    for (int i = 0; i < Player_num_players(); i++) {
        Player* p = Player_get(i);
        if (p) {
            Player_update_sprite(p);
            Dolly_render(&wizard, renderer, cam);
            float name_x, name_y;
            Camera_transform_point(cam, p->pos.x, p->pos.y, &name_x, &name_y);
            //render player's name
            int fontwidth = FC_GetWidth(Fonts_getfont(8), p->name);
            FC_Draw(Fonts_getfont(8),
                renderer,
                (int)name_x - fontwidth / 2 + 4,
                (int)name_y - wizard.rect.h / 2 - 60 * Camera_get_scale(cam),
                p->name);
        }
    }
}
