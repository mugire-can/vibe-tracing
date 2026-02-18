#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <stdio.h>
#include <math.h>

static inline int clamp_int(int x, int lo, int hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static inline void write_color(FILE *out, vec3 pixel_color, int samples_per_pixel) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    double scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    fprintf(out, "%d %d %d\n",
        clamp_int((int)(256 * fmin(fmax(r, 0.0), 0.999)), 0, 255),
        clamp_int((int)(256 * fmin(fmax(g, 0.0), 0.999)), 0, 255),
        clamp_int((int)(256 * fmin(fmax(b, 0.0), 0.999)), 0, 255));
}

static inline void write_color_to_buffer(unsigned char *buf, int idx, vec3 pixel_color, int samples_per_pixel) {
    double scale = 1.0 / samples_per_pixel;
    double r = sqrt(scale * pixel_color.x);
    double g = sqrt(scale * pixel_color.y);
    double b = sqrt(scale * pixel_color.z);

    buf[idx]     = (unsigned char)clamp_int((int)(256 * fmin(fmax(r, 0.0), 0.999)), 0, 255);
    buf[idx + 1] = (unsigned char)clamp_int((int)(256 * fmin(fmax(g, 0.0), 0.999)), 0, 255);
    buf[idx + 2] = (unsigned char)clamp_int((int)(256 * fmin(fmax(b, 0.0), 0.999)), 0, 255);
}

#endif
