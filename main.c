#define _POSIX_C_SOURCE 200809L
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
#define IMAGE_WIDTH 1920
#define ASPECT_RATIO (16.0 / 9.0)
#define IMAGE_HEIGHT ((int)(IMAGE_WIDTH / ASPECT_RATIO))
#define SAMPLES_PER_PIXEL 200
#define MAX_DEPTH 100
#define NUM_THREADS 8

/* Animation configuration */
#define TOTAL_FRAMES 300
#define FPS 30
#ifndef ENABLE_ANIMATION
#define ENABLE_ANIMATION 0  /* Default: static image. Override with -DENABLE_ANIMATION=1 */
#endif

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
    tl_seed = data->seed;

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

static void build_scene(double frame_time) {
    scene_init(&world);

    /* Initialize Perlin noise before threads are spawned */
    perlin_init();

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

    /* Random small spheres with animated heights */
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = random_double();
            double base_y = 0.2 + 0.3 * sin(frame_time * 2.0 + a + b);
            vec3 center = vec3_create(a + 0.9 * random_double(), base_y, b + 0.9 * random_double());

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

    /* Three large featured spheres with rotation and movement */
    /* Glass sphere - pulsing */
    double glass_scale = 1.0 + 0.3 * sin(frame_time * 2.0);
    scene_add_sphere(&world, (sphere){
        vec3_create(0, 1, 0), glass_scale, mat_dielectric(1.5)
    });
    
    /* Diffuse sphere - rotating in orbit */
    double angle1 = frame_time * 2.0;
    vec3 orbit_pos1 = vec3_create(-4 + 2.0 * cos(angle1), 1.0 + 0.5 * sin(frame_time), 2.0 * sin(angle1));
    scene_add_sphere(&world, (sphere){
        orbit_pos1, 1.0, mat_lambertian(vec3_create(0.4, 0.2, 0.1))
    });
    
    /* Metal sphere - rotating opposite direction */
    double angle2 = frame_time * 3.0;
    vec3 orbit_pos2 = vec3_create(4 - 2.5 * cos(angle2), 1.0 + 0.3 * cos(frame_time * 1.5), -2.5 * sin(angle2));
    scene_add_sphere(&world, (sphere){
        orbit_pos2, 1.0, mat_metal(vec3_create(0.7, 0.6, 0.5), 0.0)
    });

    /* Metallic pyramid from 4 triangles - rotating */
    material pyramid_mat = mat_metal(vec3_create(0.8, 0.6, 0.2), 0.1);
    double pyr_rotation = frame_time * 1.5;
    double cos_r = cos(pyr_rotation);
    double sin_r = sin(pyr_rotation);
    
    vec3 pA = vec3_create(8 + cos_r * 1.5, 0 + sin_r * 0.8, -2);
    vec3 pB = vec3_create(10 - cos_r * 1.2, 0 + sin_r * 0.6, -2);
    vec3 pC = vec3_create(9, 0 + cos_r * 0.8, -4);
    vec3 pT = vec3_create(9 + sin_r * 0.5, 2 + cos_r * 0.5, -3);

    scene_add_triangle(&world, (triangle){pA, pB, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pB, pC, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pC, pA, pT, pyramid_mat});
    scene_add_triangle(&world, (triangle){pA, pB, pC, pyramid_mat});
}

int main(void) {
    tl_seed = (unsigned int)time(NULL);

    /* Allocate image buffer */
    image_buffer = (unsigned char *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    if (!image_buffer) {
        fprintf(stderr, "Error: Failed to allocate image buffer\n");
        return 1;
    }

#if ENABLE_ANIMATION
    fprintf(stderr, "Rendering %d frame animation (%dx%d, %d samples/pixel, %d threads)...\n",
            TOTAL_FRAMES, IMAGE_WIDTH, IMAGE_HEIGHT, SAMPLES_PER_PIXEL, NUM_THREADS);

    for (int frame = 0; frame < TOTAL_FRAMES; frame++) {
        double frame_time = (double)frame / FPS;
        
        /* Build scene for this frame */
        build_scene(frame_time);

        /* Animate camera - circular orbit around scene */
        double cam_angle = frame_time * 0.3;
        double cam_distance = 15.0 + 3.0 * sin(frame_time * 0.5);
        vec3 lookfrom = vec3_create(
            cam_distance * cos(cam_angle),
            2.0 + 1.5 * sin(frame_time * 0.7),
            cam_distance * sin(cam_angle)
        );
        vec3 lookat = vec3_create(0, 0.5, 0);
        vec3 vup = vec3_create(0, 1, 0);
        double dist_to_focus = 10.0;
        double aperture = 0.1;

        cam = camera_create(lookfrom, lookat, vup, 20.0, ASPECT_RATIO, aperture, dist_to_focus);

        /* Multi-threaded rendering for this frame */
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        pthread_t threads[NUM_THREADS];
        thread_data tdata[NUM_THREADS];
        int rows_per_thread = IMAGE_HEIGHT / NUM_THREADS;

        for (int t = 0; t < NUM_THREADS; t++) {
            tdata[t].start_row = t * rows_per_thread;
            tdata[t].end_row = (t == NUM_THREADS - 1) ? IMAGE_HEIGHT : (t + 1) * rows_per_thread;
            tdata[t].seed = (unsigned int)(time(NULL) + t + frame);
            pthread_create(&threads[t], NULL, render_thread, &tdata[t]);
        }

        for (int t = 0; t < NUM_THREADS; t++) {
            pthread_join(threads[t], NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed = (end_time.tv_sec - start_time.tv_sec)
                       + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        /* Write PPM frame to file */
        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%04d.ppm", frame);
        FILE *f = fopen(filename, "wb");
        if (!f) {
            fprintf(stderr, "Error: Failed to open %s\n", filename);
            free(image_buffer);
            return 1;
        }

        fprintf(f, "P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
        for (int j = 0; j < IMAGE_HEIGHT; j++) {
            for (int i = 0; i < IMAGE_WIDTH; i++) {
                int idx = (j * IMAGE_WIDTH + i) * 3;
                fprintf(f, "%d %d %d\n", image_buffer[idx], image_buffer[idx + 1], image_buffer[idx + 2]);
            }
        }
        fclose(f);

        fprintf(stderr, "Frame %d/%d complete in %.2f seconds.\n", frame + 1, TOTAL_FRAMES, elapsed);
    }

#else
    /* Single frame render */
    build_scene(0.0);

    vec3 lookfrom = vec3_create(13, 2, 3);
    vec3 lookat = vec3_create(0, 0, 0);
    vec3 vup = vec3_create(0, 1, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.1;

    cam = camera_create(lookfrom, lookat, vup, 20.0, ASPECT_RATIO, aperture, dist_to_focus);

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

    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            int idx = (j * IMAGE_WIDTH + i) * 3;
            printf("%d %d %d\n", image_buffer[idx], image_buffer[idx + 1], image_buffer[idx + 2]);
        }
    }
#endif

    free(image_buffer);
    fprintf(stderr, "Done.\n");
    return 0;
}
