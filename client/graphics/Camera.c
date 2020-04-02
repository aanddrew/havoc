#include "Camera.h"

void Camera_init(Camera* self) {
    self->x = 0.0f;
    self->y = 0.0f;
    self->w = 400.0f;
    self->h = 225.0f;
    self->scale = 1.0f;

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

void Camera_set_size(Camera* self, float w, float h) {
    self->w = w;
    self->h = w;
    self->aspect_ratio = w / h;
}

void Camera_zoom(Camera* self, float multi) {
    float x,y;
    Camera_get_center(self, &x, &y);

    self->w /= multi;
    self->h /= multi;

    Camera_set_center(self, x, y);
    self->scale *= multi;
}

void Camera_transform_point(const Camera* self, float in_x, float in_y, float* out_x, float* out_y) {
    *out_x = (in_x - self->x) * self->scale;
    *out_y = (in_y - self->y) * self->scale;
}

void Camera_transform_rect(const Camera* self, const SDL_Rect* input, SDL_Rect* output) {
    output->x = (input->x - self->x) * self->scale;
    output->y = (input->y - self->y) * self->scale;
    output->w = input->w * self->scale;
    output->h = input->h * self->scale;
}

void Camera_print(const Camera* self) {
    printf("%f, %f, %f, %f, %f, %f\n", self->x, self->y, self->w, self->h, self->scale, self->aspect_ratio);
}
