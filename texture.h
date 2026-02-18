#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec3.h"
#include <math.h>

typedef enum {
    TEXTURE_SOLID,
    TEXTURE_CHECKER,
    TEXTURE_PERLIN
} texture_type;

typedef struct {
    texture_type type;
    color color1;
    color color2;
    double scale;
} texture;

static inline double perlin_noise(point3 p) {
    // Simple noise function
    double x = sin(p.x) * cos(p.y);
    double y = sin(p.y) * cos(p.z);
    double z = sin(p.z) * cos(p.x);
    return (x + y + z) / 3.0;
}

static inline color texture_value(texture *tex, double u, double v, point3 p) {
    if (tex->type == TEXTURE_SOLID) {
        return tex->color1;
    }
    else if (tex->type == TEXTURE_CHECKER) {
        double sines = sin(tex->scale * p.x) * sin(tex->scale * p.y) * sin(tex->scale * p.z);
        if (sines < 0)
            return tex->color1;
        else
            return tex->color2;
    }
    else if (tex->type == TEXTURE_PERLIN) {
        double noise = perlin_noise(vec3_mul(p, tex->scale));
        return vec3_mul(tex->color1, 0.5 * (1.0 + noise));
    }
    
    return tex->color1;
}

#endif
