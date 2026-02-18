#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "material.h"

#define MAX_SPHERES 500

typedef struct {
    sphere spheres[MAX_SPHERES];
    int count;
} scene;

static inline void scene_init(scene *s) {
    s->count = 0;
}

static inline void scene_add_sphere(scene *s, sphere sp) {
    if (s->count < MAX_SPHERES) {
        s->spheres[s->count++] = sp;
    }
}

static inline int scene_hit(scene *s, ray *r, double t_min, double t_max, hit_record *rec) {
    hit_record temp_rec;
    int hit_anything = 0;
    double closest_so_far = t_max;
    
    for (int i = 0; i < s->count; i++) {
        if (sphere_hit(&s->spheres[i], r, t_min, closest_so_far, &temp_rec)) {
            hit_anything = 1;
            closest_so_far = temp_rec.t;
            *rec = temp_rec;
        }
    }
    
    return hit_anything;
}

#endif
