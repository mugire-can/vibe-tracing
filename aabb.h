#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include <math.h>

typedef struct {
    point3 min;
    point3 max;
} aabb;

static inline aabb aabb_create(point3 min, point3 max) {
    return (aabb){min, max};
}

static inline aabb aabb_surrounding_box(aabb box0, aabb box1) {
    point3 small = {
        fmin(box0.min.x, box1.min.x),
        fmin(box0.min.y, box1.min.y),
        fmin(box0.min.z, box1.min.z)
    };
    
    point3 big = {
        fmax(box0.max.x, box1.max.x),
        fmax(box0.max.y, box1.max.y),
        fmax(box0.max.z, box1.max.z)
    };
    
    return aabb_create(small, big);
}

static inline int aabb_hit(aabb *box, ray *r, double t_min, double t_max) {
    for (int a = 0; a < 3; a++) {
        double inv_d = 1.0 / (a == 0 ? r->direction.x : (a == 1 ? r->direction.y : r->direction.z));
        double orig = a == 0 ? r->origin.x : (a == 1 ? r->origin.y : r->origin.z);
        double min_val = a == 0 ? box->min.x : (a == 1 ? box->min.y : box->min.z);
        double max_val = a == 0 ? box->max.x : (a == 1 ? box->max.y : box->max.z);
        
        double t0 = (min_val - orig) * inv_d;
        double t1 = (max_val - orig) * inv_d;
        
        if (inv_d < 0.0) {
            double temp = t0;
            t0 = t1;
            t1 = temp;
        }
        
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        
        if (t_max <= t_min)
            return 0;
    }
    return 1;
}

static inline aabb sphere_bounding_box(sphere *s) {
    point3 min = {
        s->center.x - s->radius,
        s->center.y - s->radius,
        s->center.z - s->radius
    };
    point3 max = {
        s->center.x + s->radius,
        s->center.y + s->radius,
        s->center.z + s->radius
    };
    return aabb_create(min, max);
}

#endif
