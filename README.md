# Vibe Tracing

A high-performance raytracer written in C with advanced features and multi-threading support.

## Quick Start

**New to this project?** Start here:
- 📚 **[Getting Started Guide](GETTING_STARTED.md)** - Installation and first render
- 🎬 **[Animation Guide](ANIMATION_GUIDE.md)** - Create dynamic ray-traced videos
- 🔍 **[Project Overview](PROJECT_OVERVIEW.md)** - Architecture and design philosophy
- 🐛 **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions
- 🤝 **[Contributing](CONTRIBUTING.md)** - Development guidelines

**Quick commands:**
```bash
make run        # Create single high-quality frame (~4.5 min)
make animate    # Create 300-frame animation (10 sec video, ~3-4 hours)
make video      # Convert frames to MP4 (requires ffmpeg)
```

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

### Animation Features ✨ NEW
- **Dynamic camera movement**: Orbiting, height variation, zoom effects
- **Animated objects**: 
  - Pulsing spheres (breathe in/out)
  - Orbiting spheres with multiple speeds
  - Rotating geometric primitives
  - Sinusoidal floating motion
- **Multi-frame rendering**: 300 frames at 30fps for smooth 10-second videos
- **Video export**: Automatic MP4 creation with FFmpeg

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

## Building & Running

Build the optimized raytracer:
```bash
make              # Compile
make run          # Single frame (1600x900, 100 samples, ~4.5 min)
make animate      # 300-frame animation (~2.5-4 hours)
make video        # Convert frames to MP4
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

### Single Frame Rendering

Generate a high-quality raytraced image:
```bash
make run          # Creates output.ppm (1600x900, 100 samples/pixel)
```

View or convert the output:
```bash
convert output.ppm output.png
display output.ppm
```

### Animation & Video Creation

For stunning animated ray-traced videos with dynamic lighting and moving objects:

```bash
make animate      # Renders 300 frames (10 second video)
make video        # Convert frames to MP4 (requires ffmpeg)
```

**See [ANIMATION_GUIDE.md](ANIMATION_GUIDE.md) for detailed animation instructions**

### Direct Execution

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
├── README.md    # This file
└── docs/        # Documentation (see below)
```

## Documentation

- **[Getting Started Guide](GETTING_STARTED.md)** - Complete beginner's guide
  - Installation instructions for all platforms
  - Your first render walkthrough
  - Configuration and customization
  - Performance tips and presets

- **[Project Overview](PROJECT_OVERVIEW.md)** - Deep dive into the project
  - Architecture and design philosophy
  - Technical implementation details
  - Performance characteristics
  - Code organization and modules

- **[Troubleshooting Guide](TROUBLESHOOTING.md)** - Problem solving
  - Build issues and solutions
  - Runtime problems
  - Platform-specific issues
  - Debug mode and diagnostics

- **[Contributing Guide](CONTRIBUTING.md)** - For contributors
  - Development workflow
  - Coding standards
  - Testing guidelines
  - Pull request process

## Example Scene

The default scene includes:
- ~485 randomly placed small spheres with varied materials
- 1 ground plane with checker texture
- 3 large featured spheres (glass, diffuse, metal)
- 4 triangles forming a metallic pyramid

## License

MIT License
