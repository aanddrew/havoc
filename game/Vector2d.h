#ifndef VECTOR2D_H
#define VECTOR2D_H

//
// Utility 2d vector class
// Contains some functions for math
//

typedef struct {
  float x;
  float y;
} Vector2d;

extern Vector2d Vector2d_zero;

// returns v1 $OPERATION v2
Vector2d Vector2d_multiply(Vector2d v, float scalar);
Vector2d Vector2d_add(Vector2d v1, Vector2d v2);
Vector2d Vector2d_subtract(Vector2d v1, Vector2d v2);
Vector2d Vector2d_normalize(Vector2d v);

float Vector2d_dot(Vector2d v1, Vector2d v2);
float Vector2d_magnitude_squared(Vector2d v);
float Vector2d_magnitude(Vector2d v);
float Vector2d_angle(Vector2d v);

#endif
