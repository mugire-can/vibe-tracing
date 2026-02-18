# Vibe Tracing

A high-performance raytracer written in C with advanced features and multi-threading support.

## Features

### Geometric Primitives
- **Spheres**: Efficient ray-sphere intersection
- **Planes**: Infinite plane support for ground and walls
- **Triangles**: Möller–Trumbore intersection algorithm for mesh support

### Material Types
- **Lambertian (Diffuse)**: Realistic matte surfaces
- **Metal**: Reflective surfaces with adjustable roughness/fuzz
- **Dielectric (Glass)**: Transparent materials with refraction
  - Realistic refraction using Snell's law
  - Schlick's approximation for Fresnel effect

### Advanced Rendering Features
- **Multi-threaded rendering**: Utilizes 8 threads for optimal performance
- **Anti-aliasing**: Configurable samples per pixel (default: 50)
- **Realistic lighting**:
  - Path tracing with global illumination
  - Reflection and refraction
  - Soft shadows
  - Sky gradient background
- **Depth of field**: Adjustable aperture and focus distance
- **Gamma correction**: Proper color space handling
- **Texture support**:
  - Solid colors
  - Checker patterns
  - Perlin noise (procedural textures)

### Performance Optimizations
- **SIMD-friendly vector operations**: Inline functions with compiler hints
- **Fast-math compiler optimizations**: -O3, -march=native, -ffast-math
- **Native CPU instruction utilization**: Architecture-specific optimizations
- **Efficient memory layout**: Pre-allocated image buffer
- **Optimized ray intersection**: Early exit optimizations
- **Parallel rendering**: Multi-threaded scanline rendering

### Scene Capabilities
- Support for up to 500 spheres
- Support for up to 10 planes
- Support for up to 1000 triangles
- Procedural scene generation
- Complex materials and lighting

## Building

Build the optimized raytracer:
```bash
make
```

For debug build with symbols:
```bash
make debug
```

Clean build artifacts:
```bash
make clean
```

## Usage

Generate a high-quality raytraced image:
```bash
make run
```

This will create `output.ppm` which can be viewed with any PPM-compatible image viewer or converted to other formats using tools like ImageMagick:

```bash
convert output.ppm output.png
```

Run directly to stdout:
```bash
./raytracer > my_image.ppm
```

Run performance benchmark:
```bash
make benchmark
```

## Configuration

Edit the constants in `main.c` to adjust rendering parameters:

```c
#define IMAGE_WIDTH 1200           // Output image width
#define ASPECT_RATIO (16.0 / 9.0) // Image aspect ratio
#define SAMPLES_PER_PIXEL 50      // Anti-aliasing samples
#define MAX_DEPTH 50               // Maximum ray bounce depth
#define NUM_THREADS 8              // Number of rendering threads
```

### Recommended Settings

**Fast Preview** (~30 seconds):
- IMAGE_WIDTH: 400
- SAMPLES_PER_PIXEL: 10
- MAX_DEPTH: 20

**Balanced Quality** (~2-3 minutes):
- IMAGE_WIDTH: 1200
- SAMPLES_PER_PIXEL: 50
- MAX_DEPTH: 50

**High Quality** (~10-15 minutes):
- IMAGE_WIDTH: 1920
- SAMPLES_PER_PIXEL: 200
- MAX_DEPTH: 100

## Technical Details

### Architecture

The raytracer is organized into modular header files:

- **vec3.h**: 3D vector operations with inline functions for performance
- **ray.h**: Ray definition and operations
- **camera.h**: Camera system with depth of field
- **material.h**: Material definitions and light scattering (Lambertian, Metal, Dielectric)
- **sphere.h**: Sphere primitive and intersection tests
- **plane.h**: Plane primitive and intersection tests
- **triangle.h**: Triangle primitive with Möller–Trumbore intersection
- **aabb.h**: Axis-aligned bounding boxes for optimization
- **scene.h**: Scene management and object storage
- **color.h**: Color operations and output formatting
- **texture.h**: Texture support (solid, checker, perlin)
- **main.c**: Main rendering loop and multi-threading

### Ray Tracing Algorithm

1. **Ray Generation**: For each pixel, generate multiple rays with random offsets (anti-aliasing)
2. **Ray Intersection**: Test ray against all scene objects to find nearest hit
3. **Material Interaction**: Calculate surface color based on material properties
4. **Recursive Bouncing**: Trace reflected/refracted rays up to MAX_DEPTH bounces
5. **Color Accumulation**: Average samples and apply gamma correction
6. **Output**: Write final color to PPM format

### Performance Characteristics

The raytracer uses several optimization techniques:

- **Inline functions**: All vector and intersection operations are inlined to eliminate function call overhead
- **Compiler optimizations**:
  - `-O3`: Maximum optimization level
  - `-march=native`: Use CPU-specific instructions (SSE, AVX)
  - `-ffast-math`: Fast floating-point math (relaxed IEEE compliance)
- **Multi-threading**: pthread-based parallel rendering with 8 threads
- **Efficient memory**: Pre-allocated image buffer to avoid dynamic allocation during rendering
- **Early exits**: Ray intersection tests exit early when possible

### Rendering Equation

The raytracer implements the rendering equation through Monte Carlo path tracing:

```
L_o(p,ω_o) = L_e(p,ω_o) + ∫_Ω f_r(p,ω_i,ω_o) L_i(p,ω_i) (n·ω_i) dω_i
```

Where:
- L_o: Outgoing radiance
- L_e: Emitted radiance
- f_r: BRDF (Bidirectional Reflectance Distribution Function)
- L_i: Incoming radiance
- n: Surface normal
- ω: Direction vectors

## Project Structure

```
vibe-tracing/
├── vec3.h       # 3D vector math operations
├── ray.h        # Ray definition and operations
├── color.h      # Color utilities and PPM output
├── camera.h     # Camera with depth of field
├── material.h   # Materials (Lambertian, Metal, Dielectric)
├── texture.h    # Textures (solid, checker, perlin noise)
├── sphere.h     # Sphere intersection
├── plane.h      # Plane intersection
├── triangle.h   # Triangle intersection (Möller-Trumbore)
├── aabb.h       # Axis-aligned bounding boxes
├── scene.h      # Scene management
├── main.c       # Multi-threaded rendering loop
├── Makefile     # Build system
└── README.md    # This file
```

## Example Scene

The default scene includes:
- ~485 randomly placed small spheres with varied materials
- 1 ground plane with checker texture
- 3 large featured spheres (glass, diffuse, metal)
- 4 triangles forming a metallic pyramid

## License

MIT License
