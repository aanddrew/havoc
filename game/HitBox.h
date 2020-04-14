#ifndef HITBOX_H
#define HITBOX_H

#include "Vector2d.h"

typedef struct {
    float radius;
    Vector2d center;
} HitBox;

void HitBox_init(HitBox* self);
void HitBox_deinit(HitBox* self);

int HitBox_collision(HitBox* h1, HitBox* h2);

#endif
