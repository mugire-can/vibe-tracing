#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"

typedef struct {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
} camera;

static inline camera camera_create(vec3 lookfrom, vec3 lookat, vec3 vup,
                                     double vfov, double aspect_ratio,
                                     double aperture, double focus_dist) {
    camera cam;
    double theta = vfov * 3.14159265358979323846 / 180.0;
    double h = tan(theta / 2.0);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    cam.w = vec3_unit(vec3_sub(lookfrom, lookat));
    cam.u = vec3_unit(vec3_cross(vup, cam.w));
    cam.v = vec3_cross(cam.w, cam.u);

    cam.origin = lookfrom;
    cam.horizontal = vec3_scale(cam.u, viewport_width * focus_dist);
    cam.vertical = vec3_scale(cam.v, viewport_height * focus_dist);
    cam.lower_left_corner = vec3_sub(
        vec3_sub(
            vec3_sub(cam.origin, vec3_scale(cam.horizontal, 0.5)),
            vec3_scale(cam.vertical, 0.5)),
        vec3_scale(cam.w, focus_dist));
    cam.lens_radius = aperture / 2.0;

    return cam;
}

static inline ray camera_get_ray(camera *cam, double s, double t) {
    vec3 rd = vec3_scale(random_in_unit_disk(), cam->lens_radius);
    vec3 offset = vec3_add(vec3_scale(cam->u, rd.x), vec3_scale(cam->v, rd.y));
    return ray_create(
        vec3_add(cam->origin, offset),
        vec3_sub(
            vec3_add(
                vec3_add(cam->lower_left_corner, vec3_scale(cam->horizontal, s)),
                vec3_scale(cam->vertical, t)),
            vec3_add(cam->origin, offset)));
}

#endif
