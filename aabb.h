#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"

typedef struct {
    vec3 min;
    vec3 max;
} aabb;

static inline aabb aabb_create(vec3 min, vec3 max) {
    return (aabb){min, max};
}

static inline int aabb_hit(aabb box, ray r, double t_min, double t_max) {
    /* X axis */
    double invD = 1.0 / r.direction.x;
    double t0 = (box.min.x - r.origin.x) * invD;
    double t1 = (box.max.x - r.origin.x) * invD;
    if (invD < 0.0) { double tmp = t0; t0 = t1; t1 = tmp; }
    if (t0 > t_min) t_min = t0;
    if (t1 < t_max) t_max = t1;
    if (t_max <= t_min) return 0;

    /* Y axis */
    invD = 1.0 / r.direction.y;
    t0 = (box.min.y - r.origin.y) * invD;
    t1 = (box.max.y - r.origin.y) * invD;
    if (invD < 0.0) { double tmp = t0; t0 = t1; t1 = tmp; }
    if (t0 > t_min) t_min = t0;
    if (t1 < t_max) t_max = t1;
    if (t_max <= t_min) return 0;

    /* Z axis */
    invD = 1.0 / r.direction.z;
    t0 = (box.min.z - r.origin.z) * invD;
    t1 = (box.max.z - r.origin.z) * invD;
    if (invD < 0.0) { double tmp = t0; t0 = t1; t1 = tmp; }
    if (t0 > t_min) t_min = t0;
    if (t1 < t_max) t_max = t1;
    if (t_max <= t_min) return 0;

    return 1;
}

static inline aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small_v = {
        fmin(box0.min.x, box1.min.x),
        fmin(box0.min.y, box1.min.y),
        fmin(box0.min.z, box1.min.z)
    };
    vec3 big_v = {
        fmax(box0.max.x, box1.max.x),
        fmax(box0.max.y, box1.max.y),
        fmax(box0.max.z, box1.max.z)
    };
    return aabb_create(small_v, big_v);
}

#endif
