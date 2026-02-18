#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"
#include <stdlib.h>
#include <math.h>

typedef enum {
    MATERIAL_LAMBERTIAN,
    MATERIAL_METAL,
    MATERIAL_DIELECTRIC,
    MATERIAL_EMISSIVE
} material_type;

typedef struct {
    material_type type;
    color albedo;
    double fuzz;        // For metal
    double ref_idx;     // For dielectric
    color emission;     // For emissive materials
    double emission_strength; // For emissive materials
} material;

typedef struct {
    int hit;
    point3 p;
    vec3 normal;
    double t;
    int front_face;
    material mat;
    unsigned int *seed;  // Thread-local random seed
} hit_record;

static inline vec3 random_vec3(unsigned int *seed) {
    return (vec3){
        (double)rand_r(seed) / RAND_MAX,
        (double)rand_r(seed) / RAND_MAX,
        (double)rand_r(seed) / RAND_MAX
    };
}

static inline vec3 random_vec3_range(double min, double max, unsigned int *seed) {
    vec3 v = random_vec3(seed);
    double range = max - min;
    return (vec3){
        min + v.x * range,
        min + v.y * range,
        min + v.z * range
    };
}

static inline vec3 random_in_unit_sphere(unsigned int *seed) {
    int max_iterations = 100;
    for (int i = 0; i < max_iterations; i++) {
        vec3 p = random_vec3_range(-1.0, 1.0, seed);
        if (vec3_length_squared(p) < 1.0)
            return p;
    }
    // Fallback: return normalized random vector
    return vec3_normalize(random_vec3_range(-1.0, 1.0, seed));
}

static inline vec3 random_unit_vector(unsigned int *seed) {
    return vec3_normalize(random_in_unit_sphere(seed));
}

static inline double reflectance(double cosine, double ref_idx) {
    // Schlick's approximation
    double r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

static inline int material_scatter(material *mat, ray *r_in, hit_record *rec, 
                                    color *attenuation, ray *scattered) {
    unsigned int *seed = rec->seed;
    
    if (mat->type == MATERIAL_EMISSIVE) {
        // Emissive materials don't scatter light
        return 0;
    }
    else if (mat->type == MATERIAL_LAMBERTIAN) {
        vec3 scatter_direction = vec3_add(rec->normal, random_unit_vector(seed));
        
        // Catch degenerate scatter direction
        if (fabs(scatter_direction.x) < 1e-8 && 
            fabs(scatter_direction.y) < 1e-8 && 
            fabs(scatter_direction.z) < 1e-8) {
            scatter_direction = rec->normal;
        }
        
        *scattered = (ray){rec->p, vec3_normalize(scatter_direction)};
        *attenuation = mat->albedo;
        return 1;
    }
    else if (mat->type == MATERIAL_METAL) {
        vec3 reflected = vec3_reflect(r_in->direction, rec->normal);
        *scattered = (ray){
            rec->p, 
            vec3_normalize(vec3_add(reflected, vec3_mul(random_in_unit_sphere(seed), mat->fuzz)))
        };
        *attenuation = mat->albedo;
        return vec3_dot(scattered->direction, rec->normal) > 0;
    }
    else if (mat->type == MATERIAL_DIELECTRIC) {
        *attenuation = (color){1.0, 1.0, 1.0};
        double refraction_ratio = rec->front_face ? (1.0 / mat->ref_idx) : mat->ref_idx;
        
        double cos_theta = fmin(vec3_dot(vec3_negate(r_in->direction), rec->normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        
        int cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > ((double)rand_r(seed) / RAND_MAX)) {
            direction = vec3_reflect(r_in->direction, rec->normal);
        } else {
            direction = vec3_refract(r_in->direction, rec->normal, refraction_ratio);
        }
        
        *scattered = (ray){rec->p, direction};
        return 1;
    }
    
    return 0;
}

static inline color material_emit(material *mat) {
    if (mat->type == MATERIAL_EMISSIVE) {
        return vec3_mul(mat->emission, mat->emission_strength);
    }
    return (color){0, 0, 0};
}

#endif
