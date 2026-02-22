#ifndef PLANE_H
#define PLANE_H

#include "vec3.h"
#include "ray.h"
#include "material.h"

typedef struct {
    vec3 point;
    vec3 normal;
    material mat;
} plane;

static inline int plane_hit(plane pl, ray r, double t_min, double t_max, hit_record *rec) {
    double denom = vec3_dot(pl.normal, r.direction);
    if (fabs(denom) < 1e-8) return 0;

    double t = vec3_dot(vec3_sub(pl.point, r.origin), pl.normal) / denom;
    if (t < t_min || t > t_max) return 0;

    rec->t = t;
    rec->p = ray_at(r, t);
    set_face_normal(rec, r, pl.normal);
    rec->mat = pl.mat;
    return 1;
}

#endif
