# Getting Started with Vibe Tracing

This guide will help you get up and running with the Vibe Tracing raytracer, from installation to creating your first rendered image.

## What is Vibe Tracing?

Vibe Tracing is a high-performance raytracer written in C that creates photorealistic images using advanced rendering techniques. It simulates how light bounces around a 3D scene to produce images with realistic reflections, refractions, shadows, and lighting.

### Key Capabilities

- **Realistic Materials**: Glass, metal, and matte surfaces with physically-based rendering
- **Advanced Effects**: Depth of field, anti-aliasing, soft shadows, and global illumination
- **High Performance**: Multi-threaded rendering with CPU optimizations (SIMD, fast-math)
- **Procedural Textures**: Checker patterns and Perlin noise
- **Simple PPM Output**: Easy to convert to PNG, JPEG, or other formats

## Prerequisites

Before you begin, make sure you have the following installed on your system:

### Required Software

1. **GCC Compiler** (version 7.0 or later)
   - Linux: Usually pre-installed or available via package manager
   - macOS: Install via Xcode Command Line Tools (`xcode-select --install`)
   - Windows: Install via MinGW-w64 or WSL

2. **Make** build tool
   - Linux: Usually pre-installed or `sudo apt install build-essential`
   - macOS: Included with Xcode Command Line Tools
   - Windows: Available via MinGW or WSL

3. **POSIX Threads (pthread)** library
   - Linux/macOS: Usually pre-installed
   - Windows: Available via MinGW-w64

### Optional Software

- **ImageMagick** or **GIMP** - For viewing/converting PPM files
  ```bash
  # Ubuntu/Debian
  sudo apt install imagemagick
  
  # macOS
  brew install imagemagick
  ```

## Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/mugire-can/vibe-tracing.git
cd vibe-tracing
```

### Step 2: Verify Your Setup

Check that you have the required tools:

```bash
gcc --version    # Should show GCC 7.0 or later
make --version   # Should show GNU Make
```

### Step 3: Build the Project

Build the optimized raytracer:

```bash
make
```

This creates an executable called `raytracer` in the current directory.

**Expected output:**
```
gcc -O3 -march=native -ffast-math -Wall -Wextra -std=c11 -o raytracer main.c -lpthread -lm
```

## Your First Render

### Quick Start

The easiest way to create your first image:

```bash
make run
```

This will:
1. Build the raytracer (if not already built)
2. Run the raytracer
3. Save the output to `output.ppm`

**Expected output:**
```
Rendering 1600x900 image with 100 samples/pixel, 8 threads...
Render complete in ~120-180 seconds.
Done.
Output written to output.ppm
```

### Animation - Creating a Dynamic Video

For a stunning animated sequence with moving objects, rotating camera, and dynamic lighting:

```bash
make animate
```

This creates **300 frames** (10 seconds at 30fps) with:
- **Camera orbiting** the scene with height variation
- **Animated spheres** - pulsing, rotating, and moving in orbital patterns
- **Bouncing small objects** with sinusoidal motion
- **Rotating pyramid** with metallic reflections
- **High-quality rendering** - 1600x900 resolution with 100 samples/pixel

**Expected time:** ~15-25 minutes for all frames

### Converting Frames to Video

To create an MP4 video from the rendered frames:

```bash
# Make sure FFmpeg is installed
sudo apt install ffmpeg

# Then run
make video
```

This creates `output.mp4` with:
- H.264 codec with high quality (CRF 18)
- 30 fps playback
- Smooth motion with interpolation

### Viewing the Output

The raytracer outputs images in PPM format. To view or convert:

#### Option 1: View with ImageMagick
```bash
display output.ppm    # On Linux
open output.ppm       # On macOS (if you have an app that supports PPM)
```

#### Option 2: Convert to PNG
```bash
convert output.ppm output.png
```

#### Option 3: Open in GIMP
GIMP can open PPM files directly: `File -> Open -> output.ppm`

### Understanding Render Time

The default settings take about **1-2 minutes** on a modern CPU. Render time depends on:
- Image resolution (1200x675 by default)
- Samples per pixel (50 by default - more = better quality but slower)
- CPU performance and core count

## Customizing Your Render

### Changing Render Quality

Edit `main.c` and modify these constants:

```c
#define IMAGE_WIDTH 1200           // Image width in pixels
#define ASPECT_RATIO (16.0 / 9.0) // 16:9 aspect ratio
#define SAMPLES_PER_PIXEL 50      // Anti-aliasing quality (higher = better)
#define MAX_DEPTH 50               // Ray bounce depth
#define NUM_THREADS 8              // CPU threads to use
```

### Preset Configurations

#### Fast Preview (Good for Testing)
```c
#define IMAGE_WIDTH 400
#define SAMPLES_PER_PIXEL 10
#define MAX_DEPTH 20
```
Render time: ~30 seconds

#### Balanced Quality (Default)
```c
#define IMAGE_WIDTH 1200
#define SAMPLES_PER_PIXEL 50
#define MAX_DEPTH 50
```
Render time: ~1-2 minutes

#### High Quality (Production)
```c
#define IMAGE_WIDTH 1920
#define SAMPLES_PER_PIXEL 200
#define MAX_DEPTH 100
```
Render time: ~10-15 minutes

### Rebuild After Changes

After modifying `main.c`:

```bash
make clean    # Remove old executable
make          # Build with new settings
make run      # Generate new image
```

## Understanding the Default Scene

The default scene includes:

1. **Ground Plane**: Checker pattern (green and white)
2. **~485 Small Spheres**: Randomly placed with various materials
   - 80% diffuse/matte (colored spheres)
   - 15% metallic (shiny spheres)
   - 5% glass (transparent spheres)
3. **Three Large Spheres**:
   - Center: Glass sphere (clear/refractive)
   - Left: Brown diffuse sphere
   - Right: Gold metallic sphere
4. **Golden Pyramid**: Made of 4 triangles

## Performance Testing

### Run a Benchmark

Time how long a render takes:

```bash
make benchmark
```

This runs the raytracer and measures execution time without saving the output.

### Performance Tips

1. **Use native CPU optimizations** (already enabled with `-march=native`)
2. **Adjust thread count** to match your CPU cores
3. **Lower quality settings** for faster iterations
4. **Use fast preview** while developing/testing

## Project Structure

```
vibe-tracing/
├── main.c          # Main rendering loop and scene setup
├── vec3.h          # 3D vector math (add, multiply, dot product, etc.)
├── ray.h           # Ray definition (origin + direction)
├── color.h         # Color utilities and PPM output
├── camera.h        # Camera with depth of field
├── material.h      # Material types (Lambertian, Metal, Dielectric)
├── texture.h       # Textures (solid color, checker, Perlin noise)
├── sphere.h        # Sphere geometry and ray intersection
├── plane.h         # Infinite plane geometry
├── triangle.h      # Triangle geometry (Möller-Trumbore algorithm)
├── aabb.h          # Axis-aligned bounding boxes
├── scene.h         # Scene object storage and hit testing
├── Makefile        # Build configuration
└── README.md       # Complete project documentation
```

## Next Steps

Now that you have the basics down:

1. **Experiment with settings** - Try different quality presets
2. **Read the README.md** - Learn about advanced features
3. **Explore the code** - The codebase is ~900 lines and well-organized
4. **Modify the scene** - Edit `build_scene()` in `main.c` to add objects
5. **Learn ray tracing** - Check out the MiniVibes.pdf for theory

## Common Issues

See the [Troubleshooting Guide](TROUBLESHOOTING.md) if you encounter problems.

## Additional Resources

- **README.md** - Complete technical documentation
- **MiniVibes.pdf** - Ray tracing theory and algorithms
- **Main Source** - `main.c` has the scene setup and is a good starting point

## Getting Help

If you run into issues:
1. Check the [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Review the code comments in the header files
3. Open an issue on GitHub with details about your problem

Happy ray tracing! 🌟
