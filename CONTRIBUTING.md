# Contributing to Vibe Tracing

Thank you for your interest in contributing to Vibe Tracing! This document provides guidelines and information for contributors.

## Table of Contents

- [Getting Started](#getting-started)
- [Code Organization](#code-organization)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Performance Considerations](#performance-considerations)
- [Submitting Changes](#submitting-changes)

## Getting Started

### Prerequisites

Before contributing, make sure you have:
1. Read the [README.md](README.md) for project overview
2. Followed the [GETTING_STARTED.md](GETTING_STARTED.md) guide
3. Successfully built and run the raytracer
4. Familiarity with C programming (C11 standard)
5. Basic understanding of ray tracing concepts

### Setting Up Development Environment

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/vibe-tracing.git
   cd vibe-tracing
   ```

3. Build in debug mode for development:
   ```bash
   make debug
   ```

4. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Code Organization

### File Structure

The project uses a modular header-only design:

```
Core Math & Utilities:
├── vec3.h      - 3D vector operations (add, multiply, cross, dot, etc.)
├── ray.h       - Ray structure (origin + direction)
├── color.h     - Color utilities and PPM output

Rendering Components:
├── camera.h    - Camera system with depth of field
├── material.h  - Material types and light scattering
├── texture.h   - Texture support (solid, checker, Perlin)

Geometry:
├── sphere.h    - Sphere primitive and intersection
├── plane.h     - Plane primitive and intersection
├── triangle.h  - Triangle primitive (Möller-Trumbore)
├── aabb.h      - Axis-aligned bounding boxes

Scene Management:
├── scene.h     - Scene storage and hit testing
└── main.c      - Rendering loop, threading, scene setup
```

### Design Principles

1. **Header-only libraries**: Most functionality in `.h` files with `static inline` functions
2. **Performance-first**: Inline functions, SIMD-friendly operations, minimize allocations
3. **Simple and readable**: Clear code over clever tricks
4. **Modular**: Each header is self-contained and focused

## Development Workflow

### Making Changes

1. **Start with fast settings** for quick iteration:
   ```c
   #define IMAGE_WIDTH 400
   #define SAMPLES_PER_PIXEL 10
   #define MAX_DEPTH 20
   ```

2. **Use debug build during development**:
   ```bash
   make debug
   ```
   - Enables address sanitizer
   - Catches memory errors
   - No optimizations (easier debugging)

3. **Test with optimized build before submitting**:
   ```bash
   make clean
   make
   make benchmark
   ```

### Common Development Tasks

#### Adding a New Geometric Primitive

1. Create a new header file (e.g., `cylinder.h`)
2. Define the structure:
   ```c
   typedef struct {
       vec3 base;
       vec3 axis;
       double radius;
       double height;
       material mat;
   } cylinder;
   ```

3. Implement ray intersection:
   ```c
   static inline int cylinder_hit(cylinder cyl, ray r, double t_min, 
                                   double t_max, hit_record *rec) {
       // Intersection algorithm
   }
   ```

4. Add to scene.h:
   ```c
   cylinder cylinders[MAX_CYLINDERS];
   int num_cylinders;
   ```

5. Update scene_hit() to test new primitive

#### Adding a New Material Type

1. Add enum value in `material.h`:
   ```c
   typedef enum {
       MAT_LAMBERTIAN,
       MAT_METAL,
       MAT_DIELECTRIC,
       MAT_EMISSIVE  // New type
   } material_type;
   ```

2. Add material properties to struct
3. Implement scattering in `material_scatter()`
4. Add constructor function (e.g., `mat_emissive()`)

#### Adding a New Texture Type

1. Add enum value in `texture.h`
2. Add texture data to union
3. Implement `texture_value()` case
4. Add constructor function

### Performance Testing

Always benchmark your changes:

```bash
# Before changes
make clean && make
time ./raytracer > before.ppm

# After changes
make clean && make
time ./raytracer > after.ppm

# Compare times and verify output quality
```

## Coding Standards

### C Style Guidelines

1. **Use C11 standard**: Modern C, but avoid obscure features
2. **Inline functions**: Use `static inline` for small, frequently-called functions
3. **Clear naming**:
   - Functions: `verb_noun()` (e.g., `vec3_add`, `ray_color`)
   - Structures: lowercase with underscores (e.g., `hit_record`)
   - Constants: UPPERCASE (e.g., `MAX_DEPTH`)

4. **Function signatures**:
   ```c
   static inline vec3 vec3_add(vec3 a, vec3 b) {
       return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
   }
   ```

5. **Error handling**: Check allocations, use stderr for errors
6. **Comments**: Explain "why", not "what"

### Code Formatting

- **Indentation**: 4 spaces (no tabs)
- **Line length**: ~100 characters max
- **Braces**: K&R style (opening brace on same line)
- **Spaces**: Around operators, after commas

Example:
```c
static inline double vec3_length(vec3 v) {
    return sqrt(vec3_length_squared(v));
}
```

### Header File Structure

```c
#ifndef MYHEADER_H
#define MYHEADER_H

#include "dependencies.h"

/* Type definitions */
typedef struct {
    // ...
} my_type;

/* Function declarations/definitions */
static inline my_type my_function(/* ... */) {
    // ...
}

#endif
```

## Testing

### Manual Testing Checklist

Before submitting, verify:

1. **Builds without warnings**:
   ```bash
   make clean && make
   ```
   Should show no warnings

2. **Produces valid output**:
   ```bash
   make run
   convert output.ppm output.png
   ```
   Visually inspect the image

3. **No memory leaks** (on Linux):
   ```bash
   valgrind --leak-check=full ./raytracer > output.ppm 2>&1 | grep "no leaks"
   ```

4. **Reasonable performance**:
   ```bash
   make benchmark
   ```
   Should complete in reasonable time (1-2 minutes for default settings)

5. **Thread safety**:
   - Run multiple times, output should be consistent
   - No race conditions or deadlocks

### Test Cases to Consider

- **Edge cases**: Empty scenes, extreme values
- **Different configurations**: Various image sizes, sample counts
- **All material types**: Test new features with all materials
- **Boundary conditions**: t_min, t_max in intersections

## Performance Considerations

### Optimization Guidelines

1. **Profile first**: Don't optimize without measuring
2. **Hot paths**: Focus on ray intersection and material scattering
3. **Inline everything**: Small, frequently-called functions should be inline
4. **Avoid allocations**: Minimize malloc/free in rendering loop
5. **Cache-friendly**: Keep data structures compact
6. **SIMD-friendly**: Use simple operations that vectorize well

### Performance Anti-Patterns

❌ **Don't**:
- Allocate memory in rendering loop
- Use complex branching in hot paths
- Call functions through pointers unnecessarily
- Use double-precision where float suffices (though we use double throughout)

✅ **Do**:
- Pre-allocate buffers
- Use early exits in intersection tests
- Inline small functions
- Keep data structures compact

### Benchmark Expectations

On a modern CPU (2020+), default settings should:
- Complete in 60-120 seconds
- Use 8 threads effectively
- Peak at ~100MB memory usage

## Submitting Changes

### Pull Request Process

1. **Update your branch**:
   ```bash
   git fetch origin
   git rebase origin/main
   ```

2. **Make sure tests pass**:
   ```bash
   make clean && make
   make run
   # Verify output looks correct
   ```

3. **Commit with clear messages**:
   ```bash
   git commit -m "Add cylinder primitive with ray intersection"
   ```

4. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

5. **Create Pull Request** on GitHub with:
   - Clear description of changes
   - Why the change is needed
   - Testing performed
   - Performance impact (if applicable)
   - Example renders (if visual changes)

### Commit Message Format

```
Short summary (50 chars or less)

Detailed explanation if needed:
- What was changed
- Why it was changed
- Any side effects or considerations

Example output or test results if relevant.
```

### PR Description Template

```markdown
## Description
Brief description of what this PR does.

## Changes
- Specific change 1
- Specific change 2

## Testing
- How you tested the changes
- Test results

## Performance Impact
- Benchmark before: X seconds
- Benchmark after: Y seconds
- Memory impact: None/Increased/Decreased

## Screenshots
If visual changes, include before/after images.
```

## Feature Ideas

Looking for contribution ideas? Consider:

### Geometry
- Cylinders, cones, tori
- Mesh loading (OBJ files)
- CSG operations (union, intersection, difference)
- Instancing for efficient duplication

### Materials
- Emissive materials (light sources)
- Subsurface scattering
- Anisotropic materials
- Clearcoat

### Rendering
- BVH (Bounding Volume Hierarchy) acceleration
- Denoising
- Adaptive sampling
- Motion blur

### Output
- PNG/JPEG output (instead of PPM)
- Progress bar
- Preview window (real-time updates)

### Optimization
- SIMD vectorization (SSE/AVX)
- GPU acceleration (OpenCL/CUDA)
- Better multi-threading load balancing
- Scene spatial partitioning

### Quality of Life
- Configuration file support
- Command-line arguments
- Scene file format
- More example scenes

## Questions?

If you have questions:
1. Check the [README.md](README.md) for technical details
2. Review existing code for examples
3. Open a GitHub issue with the `question` label

## Code of Conduct

- Be respectful and constructive
- Welcome newcomers and help them learn
- Focus on the technical merits of ideas
- Give credit where it's due

Thank you for contributing to Vibe Tracing! 🎨✨
