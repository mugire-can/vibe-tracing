#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "aabb.h"

#define MAX_SPHERES 500
#define MAX_PLANES  10
#define MAX_TRIANGLES 1000

typedef struct {
    sphere spheres[MAX_SPHERES];
    int num_spheres;
    plane planes[MAX_PLANES];
    int num_planes;
    triangle triangles[MAX_TRIANGLES];
    int num_triangles;
} scene;

static inline void scene_init(scene *s) {
    s->num_spheres = 0;
    s->num_planes = 0;
    s->num_triangles = 0;
}

static inline void scene_add_sphere(scene *s, sphere sp) {
    if (s->num_spheres < MAX_SPHERES)
        s->spheres[s->num_spheres++] = sp;
}

static inline void scene_add_plane(scene *s, plane pl) {
    if (s->num_planes < MAX_PLANES)
        s->planes[s->num_planes++] = pl;
}

static inline void scene_add_triangle(scene *s, triangle tri) {
    if (s->num_triangles < MAX_TRIANGLES)
        s->triangles[s->num_triangles++] = tri;
}

static inline int scene_hit(scene *s, ray r, double t_min, double t_max, hit_record *rec) {
    hit_record temp_rec;
    int hit_anything = 0;
    double closest_so_far = t_max;

    for (int i = 0; i < s->num_spheres; i++) {
        if (sphere_hit(s->spheres[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    for (int i = 0; i < s->num_planes; i++) {
        if (plane_hit(s->planes[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    for (int i = 0; i < s->num_triangles; i++) {
        if (triangle_hit(s->triangles[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }

    return hit_anything;
}

#endif
