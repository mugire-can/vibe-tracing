# Vibe Tracing - Animation Guide

## Quick Start

### Option 1: Static Image (Quick Test - ~4.5 minutes)

```bash
make run
```

Result: `output.ppm` file (1600x900, 100 samples, high quality)

### Option 2: Animation Movie (Epic Quality - ~2-4 hours)

```bash
make animate
```

Result: 300 frames `frame_0000.ppm` ... `frame_0299.ppm` (30fps = 10 second video)

**Approximate render times:**
- **Per frame:** ~30-40 seconds (1600x900, 100 samples)
- **Total 300 frames:** ~2.5-4 hours

## Export Animation as Video

### Create MP4 with FFmpeg

```bash
# Install FFmpeg (first time)
sudo apt install ffmpeg

# Create video
make video
```

Result: `output.mp4` (H.264, 30fps, high quality)

### Alternative: Create GIF

```bash
ffmpeg -framerate 30 -i frame_%04d.ppm -vf "scale=800:-1" animation.gif
```

## Animation Features

### Movement Types

1. **Camera Motion**
   - Circular orbit around the scene
   - Sinusoidal height variation
   - Variable distance zooming

2. **Object Animations**
   - **Glass Sphere (center):** Pulsing/breathing effect
   - **Brown Sphere (left):** Orbital motion + vertical bounce
   - **Metal Sphere (right):** Counter-rotation + sinusoidal motion
   - **Small Spheres:** Sinusoidal floating/bouncing
   - **Pyramid:** Rotating animation

3. **Lighting Effects**
   - Dynamic shadows (changing with object movement)
   - Reflections (on metal and glass surfaces)
   - Ambient light variation

## Performance Optimizations

### Fast Preview (Reduce Resolution)

Edit `main.c`:

```c
#define IMAGE_WIDTH 800        // Default: 1600
#define SAMPLES_PER_PIXEL 25   // Default: 100
```

Then compile:
```bash
make clean
make animate
```

**Expected time:** ~30-45 minutes (300 frames)

### Maximum Quality (4K Ultra)

```c
#define IMAGE_WIDTH 2400       // 4K
#define SAMPLES_PER_PIXEL 200  // Ultra detail
```

**Warning:** May take 8+ hours

## Common Tasks

### View a Specific Frame

```bash
# View frame 50
display frame_0050.ppm

# Or open with GIMP/Krita
gimp frame_0050.ppm
```

### Export Frame Range

```bash
# Export frames 100-150 as video
ffmpeg -framerate 30 -i frame_%04d.ppm -vf "select=between(n\,100\,150)" -vsync vfr partial_video.mp4
```

### Clean All Frames

```bash
make clean
```

## Troubleshooting

### FFmpeg Not Found

```bash
sudo apt update
sudo apt install ffmpeg
```

### Rendering Too Slow

1. Reduce resolution: `IMAGE_WIDTH = 800`
2. Reduce samples: `SAMPLES_PER_PIXEL = 25`
3. Reduce frames: `TOTAL_FRAMES = 150` (5 seconds)

### Insufficient Disk Space

300 frames ≈ 3-5 GB. Check available space:

```bash
du -sh *.ppm
df -h
```

Delete old frames:
```bash
rm frame_*.ppm
```

## Advanced Customization

### Modify Animation Parameters

Edit the `build_scene()` function in [main.c](main.c):

```c
/* Change rotation speed */
double angle1 = frame_time * 2.0;  // 2.0 = rotation speed

/* Change orbit radius */
vec3 orbit_pos1 = vec3_create(-4 + 2.0 * cos(angle1), ...);
//                                     ^^^ radius

/* Change height movement */
double glass_scale = 1.0 + 0.3 * sin(frame_time * 2.0);
//                           ^^^ amplitude
```

### Add New Objects

Add to the `build_scene()` function:

```c
/* Another rotating glass sphere */
double angle3 = frame_time * 1.0;
vec3 pos3 = vec3_create(5 * cos(angle3), 2 * sin(frame_time), 5 * sin(angle3));
scene_add_sphere(&world, (sphere){pos3, 0.8, mat_dielectric(1.5)});
```

Recompile:
```bash
make clean && make animate
```

## Next Steps

- [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) - Architecture and design
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Common issues
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contributing guidelines
