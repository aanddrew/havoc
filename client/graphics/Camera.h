#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

typedef struct {
    float x;
    float y;
    float w;
    float h;
    float aspect_ratio;
    float scale;
} Camera;

void Camera_init(Camera* self);

void Camera_set_center(Camera* self, float x, float y);
void Camera_get_center(const Camera* self, float* x, float* y);

void Camera_translate(Camera* self, float dx, float dy);

void Camera_set_size(Camera* self, float w, float h);
void Camera_zoom(Camera* self, float multi);

void Camera_transform_point(const Camera* self, float in_x, float in_y, float* out_x, float* out_y);
void Camera_transform_rect(const Camera* self, const SDL_Rect* input, SDL_Rect* output);

void Camera_print(const Camera* self);

#endif
