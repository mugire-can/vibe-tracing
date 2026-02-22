#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec3.h"
#include <math.h>

typedef enum {
    TEXTURE_SOLID,
    TEXTURE_CHECKER,
    TEXTURE_PERLIN
} texture_type;

/* Simple permutation table for Perlin noise */
static int perm[512];
static int perm_initialized = 0;

static inline void perlin_init(void) {
    if (perm_initialized) return;
    for (int i = 0; i < 256; i++) perm[i] = i;
    for (int i = 255; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }
    for (int i = 0; i < 256; i++) perm[256 + i] = perm[i];
    perm_initialized = 1;
}

static inline double perlin_fade(double t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static inline double perlin_lerp(double t, double a, double b) {
    return a + t * (b - a);
}

static inline double perlin_grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static inline double perlin_noise(vec3 p) {
    perlin_init();
    int X = (int)floor(p.x) & 255;
    int Y = (int)floor(p.y) & 255;
    int Z = (int)floor(p.z) & 255;
    double x = p.x - floor(p.x);
    double y = p.y - floor(p.y);
    double z = p.z - floor(p.z);
    double u = perlin_fade(x);
    double v = perlin_fade(y);
    double w = perlin_fade(z);
    int A  = perm[X] + Y,     AA = perm[A] + Z, AB = perm[A + 1] + Z;
    int B  = perm[X + 1] + Y, BA = perm[B] + Z, BB = perm[B + 1] + Z;
    return perlin_lerp(w,
        perlin_lerp(v,
            perlin_lerp(u, perlin_grad(perm[AA], x, y, z), perlin_grad(perm[BA], x - 1, y, z)),
            perlin_lerp(u, perlin_grad(perm[AB], x, y - 1, z), perlin_grad(perm[BB], x - 1, y - 1, z))),
        perlin_lerp(v,
            perlin_lerp(u, perlin_grad(perm[AA + 1], x, y, z - 1), perlin_grad(perm[BA + 1], x - 1, y, z - 1)),
            perlin_lerp(u, perlin_grad(perm[AB + 1], x, y - 1, z - 1), perlin_grad(perm[BB + 1], x - 1, y - 1, z - 1))));
}

static inline double turb(vec3 p, int depth) {
    double accum = 0.0;
    double weight = 1.0;
    for (int i = 0; i < depth; i++) {
        accum += weight * perlin_noise(p);
        weight *= 0.5;
        p = vec3_scale(p, 2.0);
    }
    return fabs(accum);
}

typedef struct {
    texture_type type;
    vec3 color1;
    vec3 color2;
    double scale;
} texture;

static inline texture texture_solid(vec3 color) {
    return (texture){TEXTURE_SOLID, color, {0, 0, 0}, 1.0};
}

static inline texture texture_checker(vec3 c1, vec3 c2, double scale) {
    return (texture){TEXTURE_CHECKER, c1, c2, scale};
}

static inline texture texture_perlin(vec3 c1, vec3 c2, double scale) {
    return (texture){TEXTURE_PERLIN, c1, c2, scale};
}

static inline vec3 texture_value(texture tex, vec3 p) {
    switch (tex.type) {
        case TEXTURE_CHECKER: {
            double sines = sin(tex.scale * p.x) * sin(tex.scale * p.y) * sin(tex.scale * p.z);
            return sines < 0 ? tex.color1 : tex.color2;
        }
        case TEXTURE_PERLIN: {
            double n = 0.5 * (1.0 + sin(tex.scale * p.z + 10.0 * turb(p, 7)));
            return vec3_add(vec3_scale(tex.color1, 1.0 - n), vec3_scale(tex.color2, n));
        }
        default:
            return tex.color1;
    }
}

#endif
