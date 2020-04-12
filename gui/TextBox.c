#include "TextBox.h"
#include "Fonts.h"

static int last_blink_time;
#define BLINK_INTERVAL 350
int is_blinking = 0;
static SDL_Rect cursor_rect;
static SDL_Color cursor_color;

int init_counts = 0;

void TextBox_init(TextBox* self, const char* placeholder, int font_size)
{
    self->placeholder = placeholder;
    self->num_chars = 0;
    self->x = 0;
    self->y = 0;
    self->font_size = font_size;

    self->is_active = 0;

    if (init_counts == 0) {
        cursor_rect.w = 5;
        cursor_color.r = 255;
        cursor_color.g = 255;
        cursor_color.b = 255;
        cursor_color.a = 255;
        last_blink_time = SDL_GetTicks();
    }
    for (int i = 0; i < TEXTBOX_BUFFER_SIZE; i++) {
        self->buffer[i] = '\0';
    }
    self->is_hidden = 0;
    init_counts++;
}
void TextBox_deinit(TextBox* self)
{
    if (!self) {
        printf("Deinitting null TextBox\n");
    }
}

void TextBox_insert(TextBox* self, const char* text, int start)
{
    if (start + strlen(text) > TEXTBOX_BUFFER_SIZE) {
        printf("Error: inserting too many characters into text box\n");
        return;
    }
    for (int i = 0; i < (int)strlen(text); i++) {
        self->buffer[i + start] = text[i];
    }
    self->num_chars += strlen(text);
}

void TextBox_append(TextBox* self, const char* text)
{
    if (self->num_chars + strlen(text) >= TEXTBOX_BUFFER_SIZE) {
        return;
    }
    for (int i = 0; i < (int)strlen(text); i++) {
        self->buffer[i + self->num_chars] = text[i];
    }
    self->num_chars += strlen(text);
}

void TextBox_set(TextBox* self, char c, int index)
{
    if (index < 0 || index >= TEXTBOX_BUFFER_SIZE)
        return;

    self->buffer[index] = c;
}

void TextBox_append_char(TextBox* self, char c)
{
    if (self->num_chars == TEXTBOX_BUFFER_SIZE - 1) {
        return;
    }
    self->buffer[self->num_chars] = c;
    self->num_chars++;
    self->buffer[self->num_chars] = '\0';
}

void TextBox_delete_end(TextBox* self)
{
    if (self->num_chars == 0)
        return;
    self->buffer[self->num_chars - 1] = '\0';
    self->num_chars--;
}

const char* TextBox_gettext(TextBox* self)
{
    if (self->num_chars > 0) {
        return self->buffer;
    }
    return self->placeholder;
}

void TextBox_render(TextBox* self, SDL_Renderer* renderer)
{
    if (self->is_hidden)
        return;
    //check if we should flip the cursor
    if (SDL_GetTicks() > (unsigned int)last_blink_time + BLINK_INTERVAL) {
        is_blinking = !is_blinking;
        last_blink_time = SDL_GetTicks();
    }

    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    FC_Font* font = Fonts_getfont(self->font_size);

    //make sure to cut the text off at the end of the box
    char temp;
    int maxchars;
    if (!self->is_active && self->box_width != 0) {
        maxchars = self->box_width / FC_GetWidth(font, "A");
        temp = self->buffer[maxchars];
        self->buffer[maxchars] = '\0';
    }

    //actually draw the text
    SDL_Color old_color = FC_GetDefaultColor(font);
    if (self->num_chars == 0) {
        FC_SetDefaultColor(font, FC_MakeColor(75, 75, 75, 255));
    }

    int self_x, self_y;
    TextBox_get_screen_coords(self, renderer, &self_x, &self_y);
    FC_Draw(font,
        renderer,
        self_x,
        self_y,
        TextBox_gettext(self));
    FC_SetDefaultColor(font, old_color);

    //put the temp char back in
    if (!self->is_active && self->box_width != 0) {
        self->buffer[maxchars] = temp;
    }

    //draw the blinking cursor
    if (is_blinking && self->is_active) {
        int font_width = FC_GetWidth(font, self->buffer);
        int font_height = FC_GetLineHeight(font);

        cursor_rect.x = self_x + font_width + 1;
        cursor_rect.y = self_y + 1;
        cursor_rect.h = font_height - 2;

        SDL_SetRenderDrawColor(renderer,
            cursor_color.r,
            cursor_color.g,
            cursor_color.b,
            cursor_color.a);
        SDL_RenderFillRect(renderer, &cursor_rect);
    }
}

void TextBox_render_bg(TextBox* self, SDL_Renderer* renderer)
{
    if (self->is_hidden)
        return;
    SDL_Color black;
    black.r = 0;
    black.g = 0;
    black.b = 0;
    black.a = 255;
    TextBox_render_bg_color(self, renderer, black);
}

void TextBox_render_bg_color(TextBox* self, SDL_Renderer* renderer, SDL_Color color)
{
    if (self->is_hidden)
        return;
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);

    SDL_Rect rect;
    TextBox_get_screen_coords(self, renderer, &(rect.x), &(rect.y));
    rect.w = self->box_width;

    FC_Font* font = Fonts_getfont(self->font_size);

    rect.h = FC_GetLineHeight(font);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    TextBox_render(self, renderer);
}

void TextBox_get_screen_coords(TextBox* self, SDL_Renderer* renderer, int* x, int* y)
{
    int screen_w, screen_h;
    SDL_GetRendererOutputSize(renderer, &screen_w, &screen_h);
    *x = self->x + (self->x < 0 ? screen_w : 0);
    *y = self->y + (self->y < 0 ? screen_h : 0);
}

int TextBox_is_mouse_inside(TextBox* self, SDL_Renderer* renderer, int x, int y)
{
    FC_Font* font = Fonts_getfont(self->font_size);

    int height = FC_GetLineHeight(font);
    int width = (self->box_width != 0 ? self->box_width : FC_GetWidth(font, TextBox_gettext(self)));
    int self_x, self_y;
    TextBox_get_screen_coords(self, renderer, &self_x, &self_y);

    return (x >= self_x) && (x <= self_x + width) && (y >= self_y) && (y <= self_y + height);
}
