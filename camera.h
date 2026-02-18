#ifndef CAMERA_H
#define CAMERA_H

#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "vec3.h"
#include "ray.h"
#include <stdlib.h>

typedef struct {
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
} camera;

static inline vec3 random_in_unit_disk() {
    while (1) {
        vec3 p = (vec3){
            2.0 * ((double)rand() / RAND_MAX) - 1.0,
            2.0 * ((double)rand() / RAND_MAX) - 1.0,
            0.0
        };
        if (vec3_length_squared(p) < 1.0)
            return p;
    }
}

static inline camera camera_create(point3 lookfrom, point3 lookat, vec3 vup, 
                                    double vfov, double aspect_ratio, 
                                    double aperture, double focus_dist) {
    camera cam;
    double theta = vfov * M_PI / 180.0;
    double h = tan(theta / 2.0);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    cam.w = vec3_normalize(vec3_sub(lookfrom, lookat));
    cam.u = vec3_normalize(vec3_cross(vup, cam.w));
    cam.v = vec3_cross(cam.w, cam.u);

    cam.origin = lookfrom;
    cam.horizontal = vec3_mul(cam.u, viewport_width * focus_dist);
    cam.vertical = vec3_mul(cam.v, viewport_height * focus_dist);
    cam.lower_left_corner = vec3_sub(
        vec3_sub(
            vec3_sub(cam.origin, vec3_div(cam.horizontal, 2.0)),
            vec3_div(cam.vertical, 2.0)
        ),
        vec3_mul(cam.w, focus_dist)
    );
    cam.lens_radius = aperture / 2.0;
    
    return cam;
}

static inline ray camera_get_ray(camera *cam, double s, double t) {
    vec3 rd = vec3_mul(random_in_unit_disk(), cam->lens_radius);
    vec3 offset = vec3_add(vec3_mul(cam->u, rd.x), vec3_mul(cam->v, rd.y));
    
    return (ray){
        vec3_add(cam->origin, offset),
        vec3_normalize(vec3_sub(
            vec3_add(
                vec3_add(cam->lower_left_corner, vec3_mul(cam->horizontal, s)),
                vec3_mul(cam->vertical, t)
            ),
            vec3_add(cam->origin, offset)
        ))
    };
}

#endif
