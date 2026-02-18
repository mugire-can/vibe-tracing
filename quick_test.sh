#!/bin/bash
# Quick test script for the raytracer

echo "==================================="
echo "Vibe Tracing Quick Test"
echo "==================================="
echo ""

# Check if raytracer exists
if [ ! -f "./raytracer" ]; then
    echo "Building raytracer..."
    make
    echo ""
fi

# Create a small test render
echo "Running quick test render..."
echo "This will create a small 400x225 image for testing"
echo ""

# Backup main.c
cp main.c main.c.backup

# Temporarily modify settings for fast test
sed -i 's/#define IMAGE_WIDTH 1200/#define IMAGE_WIDTH 400/' main.c
sed -i 's/#define SAMPLES_PER_PIXEL 50/#define SAMPLES_PER_PIXEL 10/' main.c
sed -i 's/#define MAX_DEPTH 50/#define MAX_DEPTH 20/' main.c

# Rebuild
make clean > /dev/null 2>&1
make > /dev/null 2>&1

# Run test
time ./raytracer > test_render.ppm 2>&1 | grep -E "(Vibe|Image|Scene|Rendering|complete|Done)"

# Restore main.c
mv main.c.backup main.c

# Rebuild with original settings
make clean > /dev/null 2>&1
make > /dev/null 2>&1

echo ""
echo "Test render saved to: test_render.ppm"
ls -lh test_render.ppm

echo ""
echo "To view the image, use:"
echo "  - ImageMagick: convert test_render.ppm test_render.png"
echo "  - Or any PPM viewer"
