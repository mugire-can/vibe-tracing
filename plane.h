#ifndef PLANE_H
#define PLANE_H

#include "vec3.h"
#include "ray.h"
#include "material.h"
#include <math.h>

typedef struct {
    point3 point;
    vec3 normal;
    material mat;
} plane;

static inline int plane_hit(plane *p, ray *r, double t_min, double t_max, hit_record *rec) {
    double denom = vec3_dot(p->normal, r->direction);
    
    // Check if ray is parallel to plane
    if (fabs(denom) < 1e-6)
        return 0;
    
    vec3 p_to_origin = vec3_sub(p->point, r->origin);
    double t = vec3_dot(p_to_origin, p->normal) / denom;
    
    if (t < t_min || t > t_max)
        return 0;
    
    rec->t = t;
    rec->p = ray_at(*r, t);
    rec->mat = p->mat;
    
    // Set face normal
    rec->front_face = denom < 0;
    rec->normal = rec->front_face ? p->normal : vec3_negate(p->normal);
    rec->hit = 1;
    
    return 1;
}

#endif
