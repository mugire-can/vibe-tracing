#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "material.h"

#define MAX_SPHERES 500
#define MAX_PLANES 10
#define MAX_TRIANGLES 1000

typedef struct {
    sphere spheres[MAX_SPHERES];
    int sphere_count;
    plane planes[MAX_PLANES];
    int plane_count;
    triangle triangles[MAX_TRIANGLES];
    int triangle_count;
} scene;

static inline void scene_init(scene *s) {
    s->sphere_count = 0;
    s->plane_count = 0;
    s->triangle_count = 0;
}

static inline void scene_add_sphere(scene *s, sphere sp) {
    if (s->sphere_count < MAX_SPHERES) {
        s->spheres[s->sphere_count++] = sp;
    }
}

static inline void scene_add_plane(scene *s, plane pl) {
    if (s->plane_count < MAX_PLANES) {
        s->planes[s->plane_count++] = pl;
    }
}

static inline void scene_add_triangle(scene *s, triangle tri) {
    if (s->triangle_count < MAX_TRIANGLES) {
        s->triangles[s->triangle_count++] = tri;
    }
}

static inline int scene_hit(scene *s, ray *r, double t_min, double t_max, hit_record *rec) {
    hit_record temp_rec;
    temp_rec.seed = rec->seed;  // Preserve seed pointer
    int hit_anything = 0;
    double closest_so_far = t_max;
    
    // Check spheres
    for (int i = 0; i < s->sphere_count; i++) {
        if (sphere_hit(&s->spheres[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
            rec->seed = temp_rec.seed;  // Restore seed pointer
        }
    }
    
    // Check planes
    for (int i = 0; i < s->plane_count; i++) {
        if (plane_hit(&s->planes[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
            rec->seed = temp_rec.seed;  // Restore seed pointer
        }
    }
    
    // Check triangles
    for (int i = 0; i < s->triangle_count; i++) {
        if (triangle_hit(&s->triangles[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
            rec->seed = temp_rec.seed;  // Restore seed pointer
        }
    }
    
    return hit_anything;
}

#endif
