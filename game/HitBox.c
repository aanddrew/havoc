#include "HitBox.h"

#include <stdio.h>

void HitBox_init(HitBox *self) {
  self->radius = 0;
  self->center.x = 0;
  self->center.y = 0;
}

void HitBox_deinit(HitBox *self) {
  printf("HitBox_deinit not implemented, passed self: %p\n", self);
}

int HitBox_collision(HitBox *h1, HitBox *h2) {
  float dist = Vector2d_magnitude(Vector2d_subtract(h1->center, h2->center));
  return (dist <= h1->radius + h2->radius);
}
