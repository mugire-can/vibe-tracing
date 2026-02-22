#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"
#include "texture.h"

typedef enum {
    MAT_LAMBERTIAN,
    MAT_METAL,
    MAT_DIELECTRIC
} material_type;

typedef struct {
    material_type type;
    texture tex;
    double fuzz;        /* For metal */
    double ref_idx;     /* For dielectric */
} material;

typedef struct {
    vec3 p;
    vec3 normal;
    double t;
    int front_face;
    material mat;
} hit_record;

static inline void set_face_normal(hit_record *rec, ray r, vec3 outward_normal) {
    rec->front_face = vec3_dot(r.direction, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : vec3_negate(outward_normal);
}

static inline double reflectance(double cosine, double ref_idx) {
    /* Schlick's approximation */
    double r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

static inline int material_scatter(material mat, ray r_in, hit_record *rec,
                                    vec3 *attenuation, ray *scattered) {
    switch (mat.type) {
        case MAT_LAMBERTIAN: {
            vec3 scatter_dir = vec3_add(rec->normal, random_unit_vector());
            if (vec3_near_zero(scatter_dir))
                scatter_dir = rec->normal;
            *scattered = ray_create(rec->p, scatter_dir);
            *attenuation = texture_value(mat.tex, rec->p);
            return 1;
        }
        case MAT_METAL: {
            vec3 reflected = vec3_reflect(vec3_unit(r_in.direction), rec->normal);
            *scattered = ray_create(rec->p, vec3_add(reflected, vec3_scale(random_in_unit_sphere(), mat.fuzz)));
            *attenuation = texture_value(mat.tex, rec->p);
            return (vec3_dot(scattered->direction, rec->normal) > 0);
        }
        case MAT_DIELECTRIC: {
            *attenuation = vec3_create(1.0, 1.0, 1.0);
            double refraction_ratio = rec->front_face ? (1.0 / mat.ref_idx) : mat.ref_idx;
            vec3 unit_direction = vec3_unit(r_in.direction);
            double cos_theta = fmin(vec3_dot(vec3_negate(unit_direction), rec->normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
            int cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;
            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = vec3_reflect(unit_direction, rec->normal);
            else
                direction = vec3_refract(unit_direction, rec->normal, refraction_ratio);
            *scattered = ray_create(rec->p, direction);
            return 1;
        }
    }
    return 0;
}

static inline material mat_lambertian(vec3 color) {
    return (material){MAT_LAMBERTIAN, texture_solid(color), 0.0, 0.0};
}

static inline material mat_lambertian_tex(texture tex) {
    return (material){MAT_LAMBERTIAN, tex, 0.0, 0.0};
}

static inline material mat_metal(vec3 color, double fuzz) {
    return (material){MAT_METAL, texture_solid(color), fuzz < 1.0 ? fuzz : 1.0, 0.0};
}

static inline material mat_dielectric(double ref_idx) {
    return (material){MAT_DIELECTRIC, texture_solid(vec3_create(1, 1, 1)), 0.0, ref_idx};
}

#endif
