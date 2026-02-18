CC = gcc
CFLAGS = -Wall -Wextra -O3 -march=native -ffast-math -pthread -std=c11
LDFLAGS = -lm -lpthread
TARGET = raytracer
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = vec3.h ray.h color.h camera.h material.h sphere.h plane.h triangle.h scene.h texture.h aabb.h

.PHONY: all clean run benchmark help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete! Run './raytracer > output.ppm' to render."

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Rendering scene..."
	./$(TARGET) > output.ppm
	@echo "Image saved to output.ppm"
	@ls -lh output.ppm

benchmark: $(TARGET)
	@echo "Running benchmark..."
	@echo "---"
	@time ./$(TARGET) > /dev/null

clean:
	rm -f $(TARGET) $(OBJECTS) output.ppm *.ppm

# Debug build
debug: CFLAGS = -Wall -Wextra -g -pthread -std=c11 -DDEBUG
debug: clean $(TARGET)

# Fast preview build (400x225, 10 samples)
preview: CFLAGS += -DPREVIEW_MODE
preview: clean $(TARGET)
	@echo "Running fast preview..."
	./$(TARGET) > preview.ppm
	@echo "Preview saved to preview.ppm"
	@ls -lh preview.ppm

help:
	@echo "Vibe Tracing Build System"
	@echo "========================="
	@echo "make          - Build optimized raytracer"
	@echo "make run      - Build and render to output.ppm"
	@echo "make benchmark- Build and run performance test"
	@echo "make debug    - Build with debug symbols"
	@echo "make clean    - Remove build artifacts"
	@echo "make help     - Show this help message"

