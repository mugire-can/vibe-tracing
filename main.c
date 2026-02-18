#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "scene.h"

// Image settings
#define ASPECT_RATIO (16.0 / 9.0)
#define IMAGE_WIDTH 1200
#define IMAGE_HEIGHT ((int)(IMAGE_WIDTH / ASPECT_RATIO))
#define SAMPLES_PER_PIXEL 50
#define MAX_DEPTH 50
#define NUM_THREADS 8

// Global scene
scene world;
camera cam;
color *image_buffer;

typedef struct {
    int start_row;
    int end_row;
    int thread_id;
} thread_data;

color ray_color(ray *r, int depth) {
    hit_record rec;
    
    // Ray bounce limit reached
    if (depth <= 0)
        return (color){0, 0, 0};
    
    // Check if ray hits any object
    if (scene_hit(&world, r, 0.001, INFINITY, &rec)) {
        ray scattered;
        color attenuation;
        color emitted = material_emit(&rec.mat);
        
        if (material_scatter(&rec.mat, r, &rec, &attenuation, &scattered)) {
            color next_color = ray_color(&scattered, depth - 1);
            return vec3_add(emitted, vec3_mult(attenuation, next_color));
        }
        return emitted;
    }
    
    // Background gradient
    vec3 unit_direction = vec3_normalize(r->direction);
    double t = 0.5 * (unit_direction.y + 1.0);
    return vec3_add(
        vec3_mul((color){1.0, 1.0, 1.0}, 1.0 - t),
        vec3_mul((color){0.5, 0.7, 1.0}, t)
    );
}

void *render_rows(void *arg) {
    thread_data *data = (thread_data *)arg;
    unsigned int seed = time(NULL) + data->thread_id;
    
    for (int j = data->start_row; j < data->end_row; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            color pixel_color = {0, 0, 0};
            
            for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
                double u = (i + ((double)rand_r(&seed) / RAND_MAX)) / (IMAGE_WIDTH - 1);
                double v = (j + ((double)rand_r(&seed) / RAND_MAX)) / (IMAGE_HEIGHT - 1);
                
                ray r = camera_get_ray(&cam, u, v);
                color sample = ray_color(&r, MAX_DEPTH);
                pixel_color = vec3_add(pixel_color, sample);
            }
            
            image_buffer[j * IMAGE_WIDTH + i] = pixel_color;
        }
        
        // Progress indicator
        if (data->thread_id == 0 && j % 10 == 0) {
            fprintf(stderr, "\rProgress: %d%%", 
                    (int)(100.0 * (j - data->start_row) / (data->end_row - data->start_row)));
            fflush(stderr);
        }
    }
    
    return NULL;
}

void create_random_scene() {
    scene_init(&world);
    
    // Ground plane (replaces ground sphere for better performance)
    material ground_mat = {MATERIAL_LAMBERTIAN, {0.5, 0.5, 0.5}, 0, 0, {0, 0, 0}, 0};
    scene_add_plane(&world, (plane){{0, 0, 0}, {0, 1, 0}, ground_mat});
    
    // Add a light source (emissive sphere above the scene)
    material light_mat = {MATERIAL_EMISSIVE, {1.0, 1.0, 1.0}, 0, 0, {1.0, 1.0, 1.0}, 5.0};
    scene_add_sphere(&world, (sphere){{0, 8, 0}, 1.5, light_mat});
    
    // Add a pyramid using triangles
    material pyramid_mat = {MATERIAL_METAL, {0.9, 0.5, 0.1}, 0.1, 0, {0, 0, 0}, 0};
    point3 base[4] = {
        {6, 0, -2},
        {8, 0, -2},
        {8, 0, 0},
        {6, 0, 0}
    };
    point3 apex = {7, 2, -1};
    
    // Four triangular faces of pyramid
    scene_add_triangle(&world, triangle_create(base[0], base[1], apex, pyramid_mat));
    scene_add_triangle(&world, triangle_create(base[1], base[2], apex, pyramid_mat));
    scene_add_triangle(&world, triangle_create(base[2], base[3], apex, pyramid_mat));
    scene_add_triangle(&world, triangle_create(base[3], base[0], apex, pyramid_mat));
    
    // Random small spheres
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double choose_mat = (double)rand() / RAND_MAX;
            point3 center = {a + 0.9 * rand() / (double)RAND_MAX, 0.2, b + 0.9 * rand() / (double)RAND_MAX};
            
            if (vec3_length(vec3_sub(center, (point3){4, 0.2, 0})) > 0.9) {
                material sphere_mat;
                
                if (choose_mat < 0.8) {
                    // Diffuse
                    color albedo = vec3_mult(random_vec3(), random_vec3());
                    sphere_mat = (material){MATERIAL_LAMBERTIAN, albedo, 0, 0, {0, 0, 0}, 0};
                } else if (choose_mat < 0.95) {
                    // Metal
                    color albedo = random_vec3_range(0.5, 1.0);
                    double fuzz = (double)rand() / RAND_MAX * 0.5;
                    sphere_mat = (material){MATERIAL_METAL, albedo, fuzz, 0, {0, 0, 0}, 0};
                } else {
                    // Glass
                    sphere_mat = (material){MATERIAL_DIELECTRIC, {1.0, 1.0, 1.0}, 0, 1.5, {0, 0, 0}, 0};
                }
                
                scene_add_sphere(&world, (sphere){center, 0.2, sphere_mat});
            }
        }
    }
    
    // Three large spheres
    material mat1 = {MATERIAL_DIELECTRIC, {1.0, 1.0, 1.0}, 0, 1.5, {0, 0, 0}, 0};
    scene_add_sphere(&world, (sphere){{0, 1, 0}, 1.0, mat1});
    
    material mat2 = {MATERIAL_LAMBERTIAN, {0.4, 0.2, 0.1}, 0, 0, {0, 0, 0}, 0};
    scene_add_sphere(&world, (sphere){{-4, 1, 0}, 1.0, mat2});
    
    material mat3 = {MATERIAL_METAL, {0.7, 0.6, 0.5}, 0.0, 0, {0, 0, 0}, 0};
    scene_add_sphere(&world, (sphere){{4, 1, 0}, 1.0, mat3});
}

int main(void) {
    // Seed random number generator
    srand(time(NULL));
    
    fprintf(stderr, "Vibe Tracing - High Performance Raytracer\n");
    fprintf(stderr, "Image: %dx%d, Samples: %d, Max Depth: %d\n", 
            IMAGE_WIDTH, IMAGE_HEIGHT, SAMPLES_PER_PIXEL, MAX_DEPTH);
    fprintf(stderr, "Threads: %d\n\n", NUM_THREADS);
    
    // Allocate image buffer
    image_buffer = (color *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(color));
    if (!image_buffer) {
        fprintf(stderr, "Failed to allocate image buffer\n");
        return 1;
    }
    
    // Setup camera
    point3 lookfrom = {13, 2, 3};
    point3 lookat = {0, 0, 0};
    vec3 vup = {0, 1, 0};
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    
    cam = camera_create(lookfrom, lookat, vup, 20.0, ASPECT_RATIO, aperture, dist_to_focus);
    
    // Create scene
    fprintf(stderr, "Creating scene...\n");
    create_random_scene();
    fprintf(stderr, "Scene created: %d spheres, %d planes, %d triangles\n\n", 
            world.sphere_count, world.plane_count, world.triangle_count);
    
    // Render with multi-threading
    fprintf(stderr, "Rendering...\n");
    clock_t start_time = clock();
    
    pthread_t threads[NUM_THREADS];
    thread_data thread_args[NUM_THREADS];
    
    int rows_per_thread = IMAGE_HEIGHT / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i == NUM_THREADS - 1) ? IMAGE_HEIGHT : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, render_rows, &thread_args[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    fprintf(stderr, "\n\nRendering complete in %.2f seconds\n", elapsed);
    fprintf(stderr, "Writing image...\n");
    
    // Output PPM image
    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // Write from top to bottom (flip y-axis)
    for (int j = IMAGE_HEIGHT - 1; j >= 0; j--) {
        for (int i = 0; i < IMAGE_WIDTH; i++) {
            write_color(stdout, image_buffer[j * IMAGE_WIDTH + i], SAMPLES_PER_PIXEL);
        }
    }
    
    free(image_buffer);
    
    fprintf(stderr, "Done!\n");
    return 0;
}
