#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"
#include <stdlib.h>
#include <math.h>

typedef enum {
    MATERIAL_LAMBERTIAN,
    MATERIAL_METAL,
    MATERIAL_DIELECTRIC
} material_type;

typedef struct {
    material_type type;
    color albedo;
    double fuzz;        // For metal
    double ref_idx;     // For dielectric
} material;

typedef struct {
    int hit;
    point3 p;
    vec3 normal;
    double t;
    int front_face;
    material mat;
} hit_record;

static inline vec3 random_vec3() {
    return (vec3){
        (double)rand() / RAND_MAX,
        (double)rand() / RAND_MAX,
        (double)rand() / RAND_MAX
    };
}

static inline vec3 random_vec3_range(double min, double max) {
    vec3 v = random_vec3();
    double range = max - min;
    return (vec3){
        min + v.x * range,
        min + v.y * range,
        min + v.z * range
    };
}

static inline vec3 random_in_unit_sphere() {
    while (1) {
        vec3 p = random_vec3_range(-1.0, 1.0);
        if (vec3_length_squared(p) < 1.0)
            return p;
    }
}

static inline vec3 random_unit_vector() {
    return vec3_normalize(random_in_unit_sphere());
}

static inline double reflectance(double cosine, double ref_idx) {
    // Schlick's approximation
    double r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

static inline int material_scatter(material *mat, ray *r_in, hit_record *rec, 
                                    color *attenuation, ray *scattered) {
    if (mat->type == MATERIAL_LAMBERTIAN) {
        vec3 scatter_direction = vec3_add(rec->normal, random_unit_vector());
        
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
            vec3_normalize(vec3_add(reflected, vec3_mul(random_in_unit_sphere(), mat->fuzz)))
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
        
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > ((double)rand() / RAND_MAX)) {
            direction = vec3_reflect(r_in->direction, rec->normal);
        } else {
            direction = vec3_refract(r_in->direction, rec->normal, refraction_ratio);
        }
        
        *scattered = (ray){rec->p, direction};
        return 1;
    }
    
    return 0;
}

#endif
