# Troubleshooting Guide

This guide helps you resolve common issues when building and running Vibe Tracing.

## Build Issues

### "gcc: command not found"

**Problem**: GCC compiler is not installed.

**Solution**:
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential

# macOS
xcode-select --install

# Fedora/RHEL
sudo dnf install gcc gcc-c++ make
```

### "pthread.h: No such file or directory"

**Problem**: POSIX threads library is missing.

**Solution**:
- **Linux**: Usually included with GCC; install `build-essential` package
- **macOS**: Included with Xcode Command Line Tools
- **Windows**: Use MinGW-w64 or WSL (Windows Subsystem for Linux)

### Compilation Warnings About "march=native"

**Problem**: Older GCC versions or cross-compilation issues.

**Solution**: Edit the Makefile and change:
```makefile
CFLAGS = -O3 -march=native -ffast-math -Wall -Wextra -std=c11
```
to:
```makefile
CFLAGS = -O3 -ffast-math -Wall -Wextra -std=c11
```
(Remove `-march=native`)

### "undefined reference to 'pthread_create'"

**Problem**: Pthread library not linked properly.

**Solution**: Make sure the Makefile has `-lpthread` in LDFLAGS:
```makefile
LDFLAGS = -lpthread -lm
```

### "math.h functions undefined" (e.g., sqrt, pow)

**Problem**: Math library not linked.

**Solution**: Make sure the Makefile has `-lm` in LDFLAGS:
```makefile
LDFLAGS = -lpthread -lm
```

## Runtime Issues

### Program Crashes Immediately

**Symptoms**: Segmentation fault or immediate exit.

**Possible Causes**:
1. Memory allocation failure (out of memory)
2. Stack overflow from deep recursion
3. Threading issues

**Solutions**:
1. Lower the image resolution:
   ```c
   #define IMAGE_WIDTH 800  // Instead of 1200
   ```

2. Reduce max ray depth:
   ```c
   #define MAX_DEPTH 30  // Instead of 50
   ```

3. Reduce thread count:
   ```c
   #define NUM_THREADS 4  // Instead of 8
   ```

### Program Hangs or Takes Forever

**Symptoms**: Render never completes or takes hours.

**Possible Causes**:
1. Settings too high for your system
2. Infinite recursion in ray tracing
3. Thread deadlock (rare)

**Solutions**:
1. Use fast preview settings first:
   ```c
   #define IMAGE_WIDTH 400
   #define SAMPLES_PER_PIXEL 10
   #define MAX_DEPTH 20
   ```

2. Monitor progress messages on stderr:
   ```bash
   ./raytracer > output.ppm
   ```
   You should see: "Rendering WxH image with N samples/pixel..."

3. Try with single thread for debugging:
   ```c
   #define NUM_THREADS 1
   ```

### Output Image is Completely Black

**Possible Causes**:
1. Camera pointing in wrong direction
2. All objects outside camera view
3. MAX_DEPTH set to 0

**Solutions**:
1. Check camera settings in `main.c`:
   ```c
   vec3 lookfrom = vec3_create(13, 2, 3);  // Camera position
   vec3 lookat = vec3_create(0, 0, 0);     // Look at origin
   ```

2. Ensure MAX_DEPTH > 0:
   ```c
   #define MAX_DEPTH 50
   ```

### Output Image is Noisy/Grainy

**Cause**: Not enough samples per pixel.

**Solution**: Increase samples (requires longer render time):
```c
#define SAMPLES_PER_PIXEL 100  // or higher
```

### Output Image is Too Dark or Too Bright

**Possible Causes**:
1. Incorrect gamma correction
2. Scene lighting issues

**Solutions**:
1. Check that gamma correction is enabled in `color.h`
2. Adjust scene lighting in `main.c` (sky gradient values)

### "Cannot open output.ppm" or "File not found"

**Cause**: Output was sent to stdout instead of a file.

**Solution**: Use redirection:
```bash
./raytracer > output.ppm
```

Or use the Makefile:
```bash
make run
```

## Viewing Issues

### Cannot Open PPM File

**Problem**: No viewer for PPM format.

**Solutions**:
1. Convert to PNG:
   ```bash
   # Install ImageMagick if needed
   sudo apt install imagemagick  # Linux
   brew install imagemagick      # macOS
   
   # Convert
   convert output.ppm output.png
   ```

2. Use GIMP (supports PPM directly):
   - Download from https://www.gimp.org
   - File -> Open -> output.ppm

3. Use online PPM viewer:
   - Search for "ppm viewer online"

### PPM File is Corrupted or Invalid

**Symptoms**: Viewer shows error or garbled image.

**Possible Causes**:
1. Render didn't complete
2. Mixed stdout/stderr output
3. File truncated

**Solutions**:
1. Ensure render completes before viewing:
   ```bash
   make run
   # Wait for "Done." message
   ```

2. Redirect only stdout to file:
   ```bash
   ./raytracer > output.ppm 2>&1
   ```

3. Check file size:
   ```bash
   ls -lh output.ppm
   ```
   Should be several megabytes for default settings.

## Performance Issues

### Render is Very Slow

**Expected Times** (default settings on modern CPU):
- Fast preview (400x225, 10 samples): ~30 seconds
- Default (1200x675, 50 samples): ~1-2 minutes
- High quality (1920x1080, 200 samples): ~10-15 minutes

**If slower than expected**:

1. Check CPU usage:
   ```bash
   top
   # or
   htop
   ```
   Should show high CPU usage (near 100% per core)

2. Verify multi-threading is working:
   - Look for "8 threads" in output message
   - Should see 8 threads in top/htop

3. Try debug build to check for issues:
   ```bash
   make clean
   make debug
   ./raytracer > output.ppm
   ```

4. Disable thread sanitizer for production:
   - Use `make` instead of `make debug`

### High Memory Usage

**Normal Memory Usage**:
- ~50-100 MB for default scene and settings
- Scales with image resolution

**If memory usage is excessive** (>1 GB):

1. Check for memory leaks with valgrind:
   ```bash
   valgrind --leak-check=full ./raytracer > output.ppm
   ```

2. Reduce image size:
   ```c
   #define IMAGE_WIDTH 800
   ```

## Platform-Specific Issues

### macOS: "ld: library not found for -lpthread"

**Problem**: macOS doesn't have a separate pthread library.

**Solution**: Edit Makefile and change:
```makefile
LDFLAGS = -lm
```
(Remove `-lpthread` on macOS)

### Windows (WSL): "Bad file descriptor" errors

**Problem**: Line ending issues or file permissions.

**Solution**:
1. Ensure proper line endings:
   ```bash
   dos2unix Makefile main.c *.h
   ```

2. Make sure you're in WSL, not Windows CMD

### Windows (MinGW): Compilation errors

**Problem**: MinGW environment not properly configured.

**Recommendation**: Use WSL (Windows Subsystem for Linux) instead:
1. Install WSL from Microsoft Store
2. Install Ubuntu from Microsoft Store
3. Follow Linux installation steps

## Debug Mode

For detailed debugging information:

```bash
make clean
make debug
./raytracer > output.ppm
```

Debug mode enables:
- Address sanitizer (catches memory errors)
- No optimizations (-O0)
- Debug symbols (-g)

**Note**: Debug builds are much slower but help identify issues.

## Getting More Help

If your issue isn't covered here:

1. **Check the code**: The project is ~900 lines and well-commented
2. **Review README.md**: Technical details and architecture
3. **Open a GitHub Issue**: Include:
   - Operating system and version
   - GCC version (`gcc --version`)
   - Full error message
   - Steps to reproduce
   - Output of `make clean && make`

## Quick Reference

### Useful Commands

```bash
# Clean everything
make clean

# Build optimized
make

# Build for debugging
make debug

# Run and save output
make run

# Time the render
make benchmark

# Check file size
ls -lh output.ppm

# View with ImageMagick
display output.ppm

# Convert to PNG
convert output.ppm output.png

# Check for memory leaks
valgrind ./raytracer > output.ppm
```

### Common File Locations

- Source files: All in project root
- Output: `output.ppm` in project root
- Executable: `raytracer` in project root
- Temporary files: None (raytracer is self-contained)
