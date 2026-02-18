#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <math.h>
#include "vec3.h"

static inline int clamp_color(double x) {
    if (x < 0.0) return 0;
    if (x > 0.999) return 255;
    return (int)(256.0 * x);
}

static inline void write_color(FILE *out, color pixel_color, int samples_per_pixel) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;
    
    // Divide color by number of samples and gamma-correct for gamma=2.0
    double scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
    
    fprintf(out, "%d %d %d\n", clamp_color(r), clamp_color(g), clamp_color(b));
}

#endif
