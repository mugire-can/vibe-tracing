# Project Overview - Vibe Tracing

## Executive Summary

**Vibe Tracing** is a high-performance, educational ray tracing renderer written in pure C. It demonstrates photorealistic rendering techniques while maintaining code clarity and performance. The project serves as both a functional renderer capable of producing beautiful images and an educational resource for learning ray tracing concepts.

## Project Goals

1. **Education**: Provide clear, readable code that teaches ray tracing concepts
2. **Performance**: Demonstrate optimization techniques (multi-threading, SIMD-friendly code, fast-math)
3. **Completeness**: Implement a full-featured ray tracer with advanced materials and effects
4. **Simplicity**: Keep the codebase small (~900 lines) and easy to understand

## What Makes This Project Unique

### Pure C Implementation
- Written in C11 standard
- Header-only modular design
- No external dependencies beyond standard libraries (pthread, math)
- Portable across Linux, macOS, and Windows (via MinGW/WSL)

### Performance-Focused
- Multi-threaded rendering (8 threads by default)
- SIMD-friendly vector operations
- Aggressive compiler optimizations (-O3, -march=native, -ffast-math)
- Efficient memory layout with pre-allocated buffers
- Inline functions to eliminate call overhead

### Educational Value
- Clean, readable code structure
- Each concept isolated in its own header file
- Well-commented with clear algorithms
- Small enough to understand in a few hours
- Accompanies MiniVibes.pdf theory document

### Complete Feature Set
- Multiple geometry types (spheres, planes, triangles)
- Physically-based materials (Lambertian, Metal, Dielectric)
- Procedural textures (solid, checker, Perlin noise)
- Advanced effects (depth of field, anti-aliasing, global illumination)
- Realistic light transport (reflection, refraction, soft shadows)

## Technical Architecture

### Core Design Philosophy

**Header-Only Libraries**: Most functionality is in `.h` files using `static inline` functions. This:
- Allows compiler to optimize aggressively
- Eliminates function call overhead for hot paths
- Keeps code organized and modular
- Enables easy code inspection and learning

**Data-Oriented Design**: Structures are kept simple and cache-friendly:
```c
typedef struct {
    double x, y, z;
} vec3;  // 24 bytes, aligned, vectorizable
```

**No Dynamic Allocation in Hot Paths**: The rendering loop avoids malloc/free:
- Image buffer pre-allocated
- Scene objects stored in fixed arrays
- Ray structures passed by value (stack-allocated)

### Rendering Pipeline

```
1. Scene Setup
   └─> Build geometry (spheres, planes, triangles)
   └─> Configure materials and textures
   └─> Position camera

2. Image Generation
   └─> For each pixel:
       └─> Generate N sample rays (anti-aliasing)
       └─> For each ray:
           └─> Trace through scene (recursive)
           └─> Test intersection with all objects
           └─> Calculate material interaction
           └─> Accumulate color
       └─> Average samples
       └─> Apply gamma correction
       └─> Write to buffer

3. Output
   └─> Write PPM format to stdout
```

### Multi-Threading Strategy

- **Work Distribution**: Image divided into horizontal strips
- **Thread Count**: 8 threads by default (configurable)
- **Thread Safety**: Each thread:
  - Has own random seed
  - Writes to non-overlapping buffer regions
  - Reads from shared scene data (immutable after setup)
- **Load Balancing**: Last thread handles remainder rows

### Ray Tracing Algorithm

Implements Monte Carlo path tracing:

```c
vec3 ray_color(ray r, scene *world, int depth) {
    if (depth <= 0)
        return black;  // Max bounces reached
    
    if (hit = scene_intersection(r, world)) {
        scatter_ray = material_scatter(hit);
        return attenuation * ray_color(scatter_ray, depth-1);
    }
    
    return sky_gradient(r);  // Background
}
```

Key concepts:
- **Recursive ray tracing**: Rays bounce until max depth
- **Russian roulette**: Probabilistic ray termination
- **Importance sampling**: Materials guide ray direction
- **Accumulation**: Multiple samples averaged for final color

## Code Organization

### Module Breakdown

| Module | Lines | Purpose | Key Functions |
|--------|-------|---------|---------------|
| vec3.h | 121 | Vector math | add, multiply, dot, cross, normalize |
| ray.h | 19 | Ray definition | Ray creation and point-at-parameter |
| color.h | 41 | Color utilities | Gamma correction, PPM output |
| camera.h | 58 | Camera system | Ray generation, depth of field |
| material.h | 93 | Materials | Lambertian, Metal, Dielectric scattering |
| texture.h | 110 | Textures | Solid, checker, Perlin noise |
| sphere.h | 38 | Sphere geometry | Ray-sphere intersection |
| plane.h | 28 | Plane geometry | Ray-plane intersection |
| triangle.h | 42 | Triangle geometry | Möller-Trumbore algorithm |
| aabb.h | 61 | Bounding boxes | AABB intersection (optimization) |
| scene.h | 75 | Scene management | Object storage, hit testing |
| main.c | 209 | Render loop | Threading, scene setup, output |
| **Total** | **895** | | |

### Dependencies Graph

```
main.c
├─> scene.h
│   ├─> sphere.h
│   │   ├─> material.h
│   │   │   ├─> texture.h
│   │   │   │   └─> vec3.h
│   │   │   └─> ray.h
│   │   └─> ray.h
│   ├─> plane.h
│   │   └─> [material.h, ray.h]
│   └─> triangle.h
│       └─> [material.h, ray.h]
├─> camera.h
│   └─> [vec3.h, ray.h]
└─> color.h
    └─> vec3.h
```

## Performance Characteristics

### Render Time Expectations

**Hardware**: Modern 8-core CPU (e.g., Intel i7-9700K, AMD Ryzen 7)

| Configuration | Resolution | Samples | Time |
|---------------|------------|---------|------|
| Fast Preview | 400x225 | 10 | ~30s |
| Default | 1200x675 | 50 | ~90s |
| High Quality | 1920x1080 | 200 | ~10min |

### Optimization Techniques

1. **Compiler Optimizations**
   - `-O3`: Maximum optimization level
   - `-march=native`: CPU-specific instructions (AVX, SSE)
   - `-ffast-math`: Relaxed IEEE compliance for speed

2. **Code-Level Optimizations**
   - Inline functions eliminate call overhead
   - Early exits in intersection tests
   - Pre-calculated constants (e.g., 1/index)
   - Efficient random number generation

3. **Algorithm Optimizations**
   - Ray intersection early exit (closest hit only)
   - Bounding boxes for complex geometry
   - Efficient material evaluation

4. **Multi-Threading**
   - Parallel scanline rendering
   - Load-balanced work distribution
   - No thread synchronization overhead

### Bottlenecks

Primary time spent in:
1. **Ray-sphere intersection** (~40%)
2. **Material scattering** (~30%)
3. **Random number generation** (~15%)
4. **Vector math** (~15%)

## Use Cases

### Educational
- **Learning ray tracing**: Clear implementation of concepts
- **Teaching C programming**: Modern C practices and patterns
- **Computer graphics courses**: Companion to theory
- **Algorithm study**: Intersection tests, BVH, path tracing

### Practical
- **Quick renders**: Fast preview of 3D scenes
- **Reference images**: Ground truth for comparing renderers
- **Benchmark**: Performance testing of ray tracing code
- **Prototyping**: Testing rendering ideas quickly

### Experimental
- **Research**: Base for implementing new techniques
- **Optimization**: Platform for testing performance ideas
- **Extensions**: Foundation for adding features

## Limitations

### Current Limitations

1. **No BVH**: O(n) intersection test (slow for many objects)
2. **No GPU**: CPU-only rendering
3. **Limited output**: PPM format only
4. **Fixed scene**: Must recompile to change scene
5. **No progress bar**: No visual feedback during render
6. **Basic materials**: No SSS, anisotropy, etc.

### By Design

These are intentional to keep the project simple:
- No external dependencies
- No complex build system
- No GUI or real-time preview
- Limited to ~1000 objects per scene type

## Future Directions

### Potential Enhancements

**Performance**:
- BVH acceleration structure
- GPU rendering (OpenCL/CUDA)
- Better load balancing
- SIMD vectorization

**Features**:
- More geometry types (cylinder, torus, CSG)
- Mesh loading (OBJ format)
- Emissive materials (light sources)
- Advanced materials (SSS, clearcoat)

**Quality of Life**:
- Command-line arguments
- Configuration files
- PNG/JPEG output
- Progress bar
- Scene file format

## Related Projects and Resources

### Similar Projects
- **Ray Tracing in One Weekend** - Peter Shirley's classic tutorial
- **smallpt** - 99-line C++ path tracer
- **TinyRayTracer** - Educational ray tracer series

### Learning Resources
- **MiniVibes.pdf** - Included theory document
- **Ray Tracing in One Weekend series** - Free online books
- **Physically Based Rendering** - Comprehensive reference
- **Scratchapixel** - Online ray tracing tutorials

## Community and Contributing

### How to Contribute

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

Quick start:
1. Fork the repository
2. Make your changes
3. Test thoroughly
4. Submit a pull request

### Feature Requests

We welcome ideas! Open a GitHub issue with:
- Clear description of the feature
- Use case / motivation
- Example code or algorithm (if applicable)

### Bug Reports

If you find a bug:
1. Check if it's already reported
2. Provide minimal reproduction steps
3. Include system info and error messages

## License

MIT License - See LICENSE file for details.

Free to use for:
- Education
- Personal projects  
- Commercial applications
- Research
- Derivative works

## Acknowledgments

**Inspiration**: Ray Tracing in One Weekend series by Peter Shirley
**Theory**: Based on standard path tracing literature
**Optimization**: Informed by performance-oriented rendering practices

## Contact and Links

- **GitHub**: https://github.com/mugire-can/vibe-tracing
- **Issues**: Report bugs or request features on GitHub
- **Documentation**: README.md, GETTING_STARTED.md, this file

---

*Last updated: 2026-02-18*
