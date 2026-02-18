#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdio.h>

typedef struct {
    double x, y, z;
} vec3;

typedef vec3 point3;
typedef vec3 color;

// Vector operations
static inline vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3 vec3_mul(vec3 v, double t) {
    return (vec3){v.x * t, v.y * t, v.z * t};
}

static inline vec3 vec3_div(vec3 v, double t) {
    return vec3_mul(v, 1.0 / t);
}

static inline vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline double vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline double vec3_length_squared(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline double vec3_length(vec3 v) {
    return sqrt(vec3_length_squared(v));
}

static inline vec3 vec3_normalize(vec3 v) {
    double len = vec3_length(v);
    if (len < 1e-8) {
        // Handle zero-length vector
        return (vec3){0, 0, 0};
    }
    return vec3_div(v, len);
}

static inline vec3 vec3_negate(vec3 v) {
    return (vec3){-v.x, -v.y, -v.z};
}

static inline vec3 vec3_mult(vec3 a, vec3 b) {
    return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline vec3 vec3_reflect(vec3 v, vec3 n) {
    return vec3_sub(v, vec3_mul(n, 2.0 * vec3_dot(v, n)));
}

static inline vec3 vec3_refract(vec3 direction, vec3 n, double etai_over_etat) {
    double cos_theta = fmin(vec3_dot(vec3_negate(direction), n), 1.0);
    vec3 r_out_perp = vec3_mul(vec3_add(direction, vec3_mul(n, cos_theta)), etai_over_etat);
    vec3 r_out_parallel = vec3_mul(n, -sqrt(fabs(1.0 - vec3_length_squared(r_out_perp))));
    return vec3_add(r_out_perp, r_out_parallel);
}

static inline void vec3_print(vec3 v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

#endif
