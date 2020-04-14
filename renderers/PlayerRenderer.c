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
        if (p && p->is_alive) {
            Player_update_sprite(p);
            Dolly_render(&wizard, renderer, cam);
            float name_x, name_y;
            Camera_transform_point(cam, p->pos.x, p->pos.y, &name_x, &name_y);

            //render health bar
            SDL_Rect health_bar;
            health_bar.x = p->pos.x - 50;
            health_bar.y = p->pos.y - 80;
            health_bar.h = 10;
            health_bar.w = (int)p->health;

            SDL_Rect transform_health;
            Camera_transform_rect(cam, &health_bar, &transform_health);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &transform_health);

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
