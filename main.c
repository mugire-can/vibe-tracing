#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "scene.h"
#include "texture.h"

/* Rendering configuration */
#define IMAGE_WIDTH 1200
#define ASPECT_RATIO (16.0 / 9.0)
#define IMAGE_HEIGHT ((int)(IMAGE_WIDTH / ASPECT_RATIO))
#define SAMPLES_PER_PIXEL 50
#define MAX_DEPTH 50
#define NUM_THREADS 8

/* Global scene and camera */
static scene world;
static camera cam;
static unsigned char *image_buffer;

/* Thread data */
typedef struct {
    int start_row;
    int end_row;
    unsigned int seed;
} thread_data;

static vec3 ray_color(ray r, scene *world, int depth) {
    if (depth <= 0)
        return vec3_create(0, 0, 0);

    hit_record rec;
    if (scene_hit(world, r, 0.001, 1e30, &rec)) {
        ray scattered;
        vec3 attenuation;
        if (material_scatter(rec.mat, r, &rec, &attenuation, &scattered))
            return vec3_mul(attenuation, ray_color(scattered, world, depth - 1));
        return vec3_create(0, 0, 0);
    }

    /* Sky gradient */
    vec3 unit_dir = vec3_unit(r.direction);
    double t = 0.5 * (unit_dir.y + 1.0);
    return vec3_add(
        vec3_scale(vec3_create(1.0, 1.0, 1.0), 1.0 - t),
        vec3_scale(vec3_create(0.5, 0.7, 1.0), t));
}

static void *render_thread(void *arg) {
    thread_data *data = (thread_data *)arg;
    srand(data->seed);

    for (int j = data->start_row; j < data->end_row; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            vec3 pixel_color = vec3_create(0, 0, 0);
            for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
                double u = (i + random_double()) / (IMAGE_WIDTH - 1);
                double v = (j + random_double()) / (IMAGE_HEIGHT - 1);
                ray r = camera_get_ray(&cam, u, v);
                pixel_color = vec3_add(pixel_color, ray_color(r, &world, MAX_DEPTH));
            }
            int row = IMAGE_HEIGHT - 1 - j;
            int idx = (row * IMAGE_WIDTH + i) * 3;
            write_color_to_buffer(image_buffer, idx, pixel_color, SAMPLES_PER_PIXEL);
        }
    }
    return NULL;
}

static void build_scene(void) {
    scene_init(&world);

    /* Ground plane with checker pattern */
    texture ground_tex = texture_checker(
        vec3_create(0.2, 0.3, 0.1),
        vec3_create(0.9, 0.9, 0.9),
        10.0);
    scene_add_plane(&world, (plane){
        vec3_create(0, 0, 0),
        vec3_create(0, 1, 0),
        mat_lambertian_tex(ground_tex)
    });

    /* Random small spheres */
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            vec3 center = vec3_create(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if (vec3_length(vec3_sub(center, vec3_create(4, 0.2, 0))) > 0.9) {
                material mat;
                if (choose_mat < 0.8) {
                    /* Diffuse */
                    vec3 albedo = vec3_mul(vec3_random(), vec3_random());
                    mat = mat_lambertian(albedo);
                } else if (choose_mat < 0.95) {
                    /* Metal */
                    vec3 albedo = vec3_random_range(0.5, 1.0);
                    double fuzz = random_double_range(0.0, 0.5);
                    mat = mat_metal(albedo, fuzz);
                } else {
                    /* Glass */
                    mat = mat_dielectric(1.5);
                }
                scene_add_sphere(&world, (sphere){center, 0.2, mat});
            }
        }
    }

    /* Three large featured spheres */
    scene_add_sphere(&world, (sphere){
        vec3_create(0, 1, 0), 1.0, mat_dielectric(1.5)
    });
    scene_add_sphere(&world, (sphere){
        vec3_create(-4, 1, 0), 1.0, mat_lambertian(vec3_create(0.4, 0.2, 0.1))
    });
    scene_add_sphere(&world, (sphere){
        vec3_create(4, 1, 0), 1.0, mat_metal(vec3_create(0.7, 0.6, 0.5), 0.0)
    });

    /* Metallic pyramid from 4 triangles */
    material pyramid_mat = mat_metal(vec3_create(0.8, 0.6, 0.2), 0.1);
    vec3 pA = vec3_create(8, 0, -2);
    vec3 pB = vec3_create(10, 0, -2);
    vec3 pC = vec3_create(9, 0, -4);
    vec3 pT = vec3_create(9, 2, -3);

    scene_add_triangle(&world, (triangle){pA, pB, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pB, pC, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pC, pA, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pA, pB, pC, pyramid_mat});
}

int main(void) {
    srand((unsigned int)time(NULL));

    /* Build scene */
    build_scene();

    /* Camera setup */
    vec3 lookfrom = vec3_create(13, 2, 3);
    vec3 lookat = vec3_create(0, 0, 0);
    vec3 vup = vec3_create(0, 1, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.1;

    cam = camera_create(lookfrom, lookat, vup, 20.0, ASPECT_RATIO, aperture, dist_to_focus);

    /* Allocate image buffer */
    image_buffer = (unsigned char *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    if (!image_buffer) {
        fprintf(stderr, "Error: Failed to allocate image buffer\n");
        return 1;
    }

    /* Multi-threaded rendering */
    fprintf(stderr, "Rendering %dx%d image with %d samples/pixel, %d threads...\n",
            IMAGE_WIDTH, IMAGE_HEIGHT, SAMPLES_PER_PIXEL, NUM_THREADS);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pthread_t threads[NUM_THREADS];
    thread_data tdata[NUM_THREADS];
    int rows_per_thread = IMAGE_HEIGHT / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++) {
        tdata[t].start_row = t * rows_per_thread;
        tdata[t].end_row = (t == NUM_THREADS - 1) ? IMAGE_HEIGHT : (t + 1) * rows_per_thread;
        tdata[t].seed = (unsigned int)(time(NULL) + t);
        pthread_create(&threads[t], NULL, render_thread, &tdata[t]);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = (end_time.tv_sec - start_time.tv_sec)
                   + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    fprintf(stderr, "Render complete in %.2f seconds.\n", elapsed);

    /* Write PPM to stdout */
    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            int idx = (j * IMAGE_WIDTH + i) * 3;
            printf("%d %d %d\n", image_buffer[idx], image_buffer[idx + 1], image_buffer[idx + 2]);
        }
    }

    free(image_buffer);
    fprintf(stderr, "Done.\n");
    return 0;
}
