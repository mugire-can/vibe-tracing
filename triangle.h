#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"
#include "ray.h"
#include "material.h"
#include <math.h>

typedef struct {
    point3 v0, v1, v2;
    vec3 normal;
    material mat;
} triangle;

// Möller–Trumbore intersection algorithm
static inline int triangle_hit(triangle *tri, ray *r, double t_min, double t_max, hit_record *rec) {
    vec3 edge1 = vec3_sub(tri->v1, tri->v0);
    vec3 edge2 = vec3_sub(tri->v2, tri->v0);
    
    vec3 h = vec3_cross(r->direction, edge2);
    double a = vec3_dot(edge1, h);
    
    if (fabs(a) < 1e-8)
        return 0;
    
    double f = 1.0 / a;
    vec3 s = vec3_sub(r->origin, tri->v0);
    double u = f * vec3_dot(s, h);
    
    if (u < 0.0 || u > 1.0)
        return 0;
    
    vec3 q = vec3_cross(s, edge1);
    double v = f * vec3_dot(r->direction, q);
    
    if (v < 0.0 || u + v > 1.0)
        return 0;
    
    double t = f * vec3_dot(edge2, q);
    
    if (t < t_min || t > t_max)
        return 0;
    
    rec->t = t;
    rec->p = ray_at(*r, t);
    rec->mat = tri->mat;
    
    vec3 outward_normal = tri->normal;
    rec->front_face = vec3_dot(r->direction, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : vec3_negate(outward_normal);
    rec->hit = 1;
    
    return 1;
}

static inline triangle triangle_create(point3 v0, point3 v1, point3 v2, material mat) {
    vec3 edge1 = vec3_sub(v1, v0);
    vec3 edge2 = vec3_sub(v2, v0);
    vec3 normal = vec3_normalize(vec3_cross(edge1, edge2));
    return (triangle){v0, v1, v2, normal, mat};
}

#endif
