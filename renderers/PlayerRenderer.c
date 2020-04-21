#include "PlayerRenderer.h"

#include "../gui/Fonts.h"

static Dolly wizards[8];

void Player_init_all_sprites(SDL_Renderer* renderer)
{
    for (int i = 0; i < 8; i++) {
        Dolly_init_with_texture(&wizards[i], renderer, "res/wizard/wizard_sheet_16.png");
        wizards[i].srcrect.w = 16;
        wizards[i].srcrect.h = 16;

        int num_pixels = wizards[i].surface->w * wizards[i].surface->h;
        //set the colors in the surface to reflect the team of this wizard
        for (int p = 0; p < num_pixels; p++) {
            Uint8* pixel = wizards[i].surface->pixels + (p * 4);
            SDL_Color* color = (SDL_Color*)pixel;
            if (color->r > 128 && color->g > 128 && color->b > 128) {
                continue;
            }

            int new_r = (i & 1) ? 200 : color->r;
            int new_g = (i & 2) ? 150 : color->g;
            int new_b = (i & 4) ? 200 : color->b;
            color->r = new_r;
            color->g = new_g;
            color->b = new_b;
        }

        Dolly_refresh_texture_from_surface(&wizards[i], renderer);
    }
}

void Player_cleanup_all_sprites()
{
    for (int i = 0; i < 8; i++) {
        Dolly_delete(&wizards[i]);
    }
}

static void Player_update_sprite(Player* self)
{
    Dolly* wizard = &wizards[self->team];

    static const float LOOK_OFFSET = -3.0 * M_PI / 4.0;

    wizard->rect.x = (int)(self->pos.x) - wizard->rect.w / 2;
    wizard->rect.y = (int)(self->pos.y) - wizard->rect.h / 2;
    float angle = Vector2d_angle(self->look);
    wizard->angle = angle + LOOK_OFFSET;
}

void Player_render_all(SDL_Renderer* renderer, const Camera* cam)
{
    for (int i = 0; i < Player_num_players(); i++) {
        Player* p = Player_get(i);
        if (p && p->is_alive) {
            //render sprite
            Player_update_sprite(p);
            int team = p->team;
            if (team < 0 || team >= 8) {
                printf("INVALID TEAM NUMBER: %d, CAN'T RENDER CORRECT WIZARD\n", team);
                return;
            }
            Dolly_render(&wizards[team], renderer, cam);
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
                (int)name_y - wizards[team].rect.h / 2 - 60 * Camera_get_scale(cam),
                p->name);
        }
    }
}
