#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    point3 origin;
    vec3 direction;
} ray;

static inline point3 ray_at(ray r, double t) {
    return vec3_add(r.origin, vec3_mul(r.direction, t));
}

#endif
