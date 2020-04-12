#include "Camera.h"

void Camera_init(Camera* self, SDL_Renderer* renderer) {
    self->renderer = renderer;
    {
        SDL_Rect viewport;
        SDL_RenderGetViewport(renderer, &viewport);

        Camera_set_size(self, viewport.w, viewport.h);
        Camera_set_center(self, 0, 0);
    }

    int renderw, renderh;
    SDL_GetRendererOutputSize(renderer, &renderw, &renderh);

    self->w = renderw;
    self->h = renderh;

    self->aspect_ratio = self->w / self->h;
}

void Camera_set_center(Camera* self, float x, float y) {
    self->x = x - self->w/2;
    self->y = y - self->h/2;
}

void Camera_get_center(const Camera* self, float* x, float* y) {
    *x = self->x + self->w/2;
    *y = self->y + self->h/2;
}

void Camera_translate(Camera* self, float dx, float dy) {
    float x,y;
    Camera_get_center(self, &x, &y);
    Camera_set_center(self, x + dx, y + dy);
}

//Changes the dimensions of the camera
//Preserves the original center point of the camera
void Camera_set_size(Camera* self, float w, float h) {
    //capture the original center point
    float x, y;
    Camera_get_center(self, &x, &y);

    self->w = w;
    self->h = h;

    //now set the center back to that point
    self->aspect_ratio = w / h;
    Camera_set_center(self, x, y);
}

void Camera_get_mousestate_relative(const Camera*self, int* x, int* y) {
    SDL_GetMouseState(x, y);

    float scale = Camera_get_scale(self);
    *x = ((float) *x) / scale;
    *y = ((float) *y) / scale;

    *x += self->x;
    *y += self->y;
}

//multi is the factor to increase zoom level by
//so if we set multi to 2.0 it means that objects will be twice as big,
//or conversely the screen will be half the size
void Camera_zoom(Camera* self, float multi) {
    Camera_set_size(self, self->w/multi, self->h/multi);
}

float Camera_get_scale(const Camera* self) {
    int renderw, renderh;
    SDL_GetRendererOutputSize(self->renderer, &renderw, &renderh);
    return ((float) renderw) / self->w;
}

void Camera_transform_point(const Camera* self, float in_x, float in_y, float* out_x, float* out_y) {
    float scale = Camera_get_scale(self);
    *out_x = (in_x - self->x) * scale;
    *out_y = (in_y - self->y) * scale;
}

void Camera_transform_rect(const Camera* self, const SDL_Rect* input, SDL_Rect* output) {
    float scale = Camera_get_scale(self);

    output->x = (input->x - self->x) * scale;
    output->y = (input->y - self->y) * scale;
    output->w = (input->w * scale) + 1.0f;
    output->h = (input->h * scale)+ 1.0f;
}

void Camera_print(const Camera* self) {
    printf("%f, %f, %f, %f, %f\n", self->x, self->y, self->w, self->h, self->aspect_ratio);
}
