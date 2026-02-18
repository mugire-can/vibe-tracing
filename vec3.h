#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>

typedef struct {
    double x, y, z;
} vec3;

static inline vec3 vec3_create(double x, double y, double z) {
    return (vec3){x, y, z};
}

static inline vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3 vec3_mul(vec3 a, vec3 b) {
    return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline vec3 vec3_scale(vec3 v, double t) {
    return (vec3){v.x * t, v.y * t, v.z * t};
}

static inline vec3 vec3_negate(vec3 v) {
    return (vec3){-v.x, -v.y, -v.z};
}

static inline double vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline double vec3_length_squared(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline double vec3_length(vec3 v) {
    return sqrt(vec3_length_squared(v));
}

static inline vec3 vec3_unit(vec3 v) {
    double len = vec3_length(v);
    return (vec3){v.x / len, v.y / len, v.z / len};
}

static inline int vec3_near_zero(vec3 v) {
    double s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

static inline vec3 vec3_reflect(vec3 v, vec3 n) {
    return vec3_sub(v, vec3_scale(n, 2.0 * vec3_dot(v, n)));
}

static inline vec3 vec3_refract(vec3 uv, vec3 n, double etai_over_etat) {
    double cos_theta = fmin(vec3_dot(vec3_negate(uv), n), 1.0);
    vec3 r_out_perp = vec3_scale(vec3_add(uv, vec3_scale(n, cos_theta)), etai_over_etat);
    vec3 r_out_parallel = vec3_scale(n, -sqrt(fabs(1.0 - vec3_length_squared(r_out_perp))));
    return vec3_add(r_out_perp, r_out_parallel);
}

/* Random number helpers */
static inline double random_double(void) {
    return rand() / (RAND_MAX + 1.0);
}

static inline double random_double_range(double min, double max) {
    return min + (max - min) * random_double();
}

static inline vec3 vec3_random(void) {
    return (vec3){random_double(), random_double(), random_double()};
}

static inline vec3 vec3_random_range(double min, double max) {
    return (vec3){
        random_double_range(min, max),
        random_double_range(min, max),
        random_double_range(min, max)
    };
}

static inline vec3 random_in_unit_sphere(void) {
    while (1) {
        vec3 p = vec3_random_range(-1.0, 1.0);
        if (vec3_length_squared(p) < 1.0)
            return p;
    }
}

static inline vec3 random_unit_vector(void) {
    return vec3_unit(random_in_unit_sphere());
}

static inline vec3 random_in_unit_disk(void) {
    while (1) {
        vec3 p = {random_double_range(-1.0, 1.0), random_double_range(-1.0, 1.0), 0.0};
        if (vec3_length_squared(p) < 1.0)
            return p;
    }
}

#endif
