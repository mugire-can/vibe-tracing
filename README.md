# vibe-tracing

<<<<<<< copilot/work-on-c-project
A lightweight, thread-safe tracing library for C applications.

## Features

- **Thread-safe logging**: Uses mutex locks to ensure thread-safe operation
- **Multiple trace levels**: DEBUG, INFO, WARN, ERROR
- **Flexible output**: Write to stdout, stderr, or custom files
- **Timestamped logs**: Each trace includes microsecond-precision timestamps
- **Source location**: Automatic capture of file, line, and function information
- **Easy to use**: Simple macro-based API

## Building

Build the library and examples:

=======
A lightweight C library for loading and inspecting PDF files.

## Features

- Open PDF files from disk or from a memory buffer
- Validate PDF structure (header, cross-reference, EOF marker)
- Query document metadata (version, page count, file size)
- Basic text extraction from PDF streams
- Clean error handling with descriptive error codes

## Building

```sh
make          # build library and example
make test     # build and run tests
make clean    # remove build artifacts
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
>>>>>>> main
```bash
make
```

<<<<<<< copilot/work-on-c-project
Clean build artifacts:

=======
For debug build with symbols:
```bash
make debug
```

Clean build artifacts:
>>>>>>> main
```bash
make clean
```

<<<<<<< copilot/work-on-c-project
Run the example:

=======
## Usage

```c
#include "pdf_loader.h"

pdf_document_t *doc = NULL;
pdf_error_t err = pdf_open("document.pdf", &doc);
if (err != PDF_OK) {
    fprintf(stderr, "Error: %s\n", pdf_error_string(err));
    return 1;
}

printf("Version: %s\n", pdf_get_version(doc));
printf("Pages:   %d\n", pdf_page_count(doc));

char *text = NULL;
size_t len = 0;
pdf_extract_text(doc, 0, &text, &len);
printf("Text: %s\n", text);
free(text);

pdf_close(doc);
```

## API

| Function | Description |
|---|---|
| `pdf_open` | Open a PDF file from disk |
| `pdf_open_from_memory` | Load a PDF from a memory buffer |
| `pdf_page_count` | Get the number of pages |
| `pdf_get_version` | Get the PDF version string |
| `pdf_get_file_size` | Get the file size in bytes |
| `pdf_extract_text` | Extract text from a specific page |
| `pdf_error_string` | Get a human-readable error description |
| `pdf_close` | Free all resources |

## Project Structure

```
include/pdf_loader.h   - Public API header
src/pdf_loader.c       - Library implementation
examples/load_pdf.c    - Example: load and inspect a PDF
tests/test_pdf_loader.c - Unit tests
Makefile               - Build system
```
Generate a high-quality raytraced image:
>>>>>>> main
```bash
make run
```

<<<<<<< copilot/work-on-c-project
## Usage

### Basic Example

```c
#include "vibe_tracing.h"

int main() {
    // Initialize tracing (to stdout)
    vibe_trace_init("stdout");
    
    // Log messages at different levels
    TRACE_DEBUG("This is a debug message");
    TRACE_INFO("Application started");
    TRACE_WARN("This is a warning");
    TRACE_ERROR("This is an error");
    
    // Clean up
    vibe_trace_cleanup();
    return 0;
}
```

### Setting Trace Level

```c
// Only show INFO and above (filters out DEBUG)
vibe_trace_set_level(TRACE_LEVEL_INFO);
```

### Output to File

```c
// Initialize with file output
vibe_trace_init("/tmp/app.log");
```

## API Reference

### Initialization and Cleanup

- `int vibe_trace_init(const char *output_file)` - Initialize tracing system
  - Returns 0 on success, -1 on failure
  - `output_file` can be "stdout", "stderr", or a file path
  
- `void vibe_trace_cleanup(void)` - Cleanup and close trace output

### Configuration

- `void vibe_trace_set_level(trace_level_t level)` - Set minimum trace level

### Logging Macros

- `TRACE_DEBUG(format, ...)` - Log debug message
- `TRACE_INFO(format, ...)` - Log info message
- `TRACE_WARN(format, ...)` - Log warning message
- `TRACE_ERROR(format, ...)` - Log error message

## Project Structure

```
vibe-tracing/
├── include/          # Public header files
│   └── vibe_tracing.h
├── src/              # Implementation files
│   └── vibe_tracing.c
├── examples/         # Example programs
│   └── simple_trace.c
├── build/            # Build output (generated)
├── Makefile          # Build configuration
└── README.md         # This file
```

## License

This is a demonstration project.
=======
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

**Fast Preview** (30 seconds):
- IMAGE_WIDTH: 400
- SAMPLES_PER_PIXEL: 10
- MAX_DEPTH: 20

**Balanced Quality** (2-3 minutes):
- IMAGE_WIDTH: 1200
- SAMPLES_PER_PIXEL: 50
- MAX_DEPTH: 50

**High Quality** (10-15 minutes):
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

**Performance**: On a modern 8-core CPU, the default scene (485 spheres + pyramid) renders at 1200x675 with 50 samples/pixel in approximately 2 minutes.

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

## Example Scene

The default scene includes:
- 485 randomly placed small spheres with varied materials
- 1 ground plane
- 3 large featured spheres (glass, diffuse, metal)
- 4 triangles forming a metallic pyramid

## Future Enhancements

Potential features for future versions:
- Bounding Volume Hierarchy (BVH) for faster ray intersection
- Importance sampling for better convergence
- Image textures and UV mapping
- Normal mapping for surface detail
- Motion blur
- Volumetric rendering (fog, smoke)
- OBJ file loading for arbitrary meshes
- Caustics and subsurface scattering

## License

MIT License
>>>>>>> main
