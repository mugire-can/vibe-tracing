# Vibe Tracing

A high-performance raytracer written in C with advanced features and multi-threading support.

## Features

- **Multiple Material Types**
  - Lambertian (diffuse) materials
  - Metal materials with adjustable roughness
  - Dielectric materials (glass) with refraction
  
- **Advanced Rendering**
  - Multi-threaded rendering for optimal performance
  - Anti-aliasing with configurable samples per pixel
  - Realistic lighting with reflection and refraction
  - Depth of field with adjustable aperture
  - Gamma correction
  
- **Optimizations**
  - SIMD-friendly vector operations
  - Fast-math compiler optimizations
  - Native CPU instruction utilization
  - Efficient ray-sphere intersection
  
- **Scene Support**
  - Procedural scene generation
  - Multiple objects (up to 500 spheres)
  - Configurable camera system

## Building

```bash
make
```

For debug build:
```bash
make debug
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

Run performance benchmark:
```bash
make benchmark
```

## Configuration

Edit the constants in `main.c` to adjust rendering parameters:

- `IMAGE_WIDTH`: Output image width (default: 800)
- `ASPECT_RATIO`: Image aspect ratio (default: 16:9)
- `SAMPLES_PER_PIXEL`: Anti-aliasing samples (default: 100)
- `MAX_DEPTH`: Maximum ray bounce depth (default: 50)
- `NUM_THREADS`: Number of rendering threads (default: 8)

## Technical Details

### Architecture

- **vec3.h**: 3D vector operations with inline functions for performance
- **ray.h**: Ray definition and operations
- **camera.h**: Camera system with depth of field
- **material.h**: Material definitions and light scattering
- **sphere.h**: Sphere primitive and intersection tests
- **scene.h**: Scene management and object storage
- **color.h**: Color operations and output formatting
- **main.c**: Main rendering loop and multi-threading

### Performance

The raytracer uses several optimization techniques:
- Inline functions to eliminate function call overhead
- Compiler optimization flags (-O3, -march=native, -ffast-math)
- Multi-threading with pthread for parallel rendering
- Efficient memory layout with pre-allocated image buffer

On a modern CPU, the default scene renders in approximately 30-60 seconds.

## License

MIT License