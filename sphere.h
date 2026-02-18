#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "ray.h"
#include "material.h"

typedef struct {
    vec3 center;
    double radius;
    material mat;
} sphere;

static inline int sphere_hit(sphere s, ray r, double t_min, double t_max, hit_record *rec) {
    vec3 oc = vec3_sub(r.origin, s.center);
    double a = vec3_length_squared(r.direction);
    double half_b = vec3_dot(oc, r.direction);
    double c = vec3_length_squared(oc) - s.radius * s.radius;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return 0;
    double sqrtd = sqrt(discriminant);

    double root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max)
            return 0;
    }

    rec->t = root;
    rec->p = ray_at(r, rec->t);
    vec3 outward_normal = vec3_scale(vec3_sub(rec->p, s.center), 1.0 / s.radius);
    set_face_normal(rec, r, outward_normal);
    rec->mat = s.mat;
    return 1;
}

#endif
