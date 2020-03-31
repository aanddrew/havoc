#include "Vector2d.h"

#include <math.h>

Vector2d Vector2d_multiply(Vector2d v, float scalar) {
    Vector2d ret;
    ret.x = v.x * scalar;
    ret.y = v.y * scalar;
    return ret;
}

Vector2d Vector2d_add(Vector2d v1, Vector2d v2) {
    Vector2d ret;
    ret.x = v1.x + v2.x;
    ret.y = v1.y + v2.y;
    return ret;
}

Vector2d Vector2d_subtract(Vector2d v1, Vector2d v2) {
    Vector2d ret;
    ret.x = v1.x - v2.x;
    ret.y = v1.y - v2.y;
    return ret;
}

float Vector2d_dot(Vector2d v1, Vector2d v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

float Vector2d_magnitude_squared(Vector2d v) {
    return v.x*v.x + v.y*v.y;
}

float Vector2d_magnitude(Vector2d v) {
    return sqrt(Vector2d_magnitude_squared(v));
}

float Vector2d_angle(Vector2d v) {
    if (v.x == 0.0f) {
        if (v.y > 0.0f) {
            return M_PI / 2.0f;
        }
        else {
            return 3.0f * M_PI / 2.0f;
        }
    }
    float tan = v.y / v.x;
    float angle = atan(tan);
    if (v.x < 0.0f) {
        angle += M_PI;
    }
    else if (v.y < 0.0f) {
        angle += 2* M_PI;
    }
    return angle;
}
