#ifndef RAY_H
#define RAY_H

#include "vec3.h"

typedef struct {
    vec3 origin;
    vec3 direction;
} ray;

static inline ray ray_create(vec3 origin, vec3 direction) {
    return (ray){origin, direction};
}

static inline vec3 ray_at(ray r, double t) {
    return vec3_add(r.origin, vec3_scale(r.direction, t));
}

#endif
