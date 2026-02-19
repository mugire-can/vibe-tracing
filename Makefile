CC = gcc
CFLAGS = -O3 -march=native -ffast-math -Wall -Wextra -std=c11
LDFLAGS = -lpthread -lm

TARGET = raytracer
TARGET_ANIM = raytracer_anim
SRC = main.c

.PHONY: all clean run debug benchmark animate video

all: $(TARGET)

$(TARGET): $(SRC) vec3.h ray.h color.h camera.h material.h sphere.h plane.h triangle.h aabb.h scene.h texture.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

$(TARGET_ANIM): $(SRC) vec3.h ray.h color.h camera.h material.h sphere.h plane.h triangle.h aabb.h scene.h texture.h
	$(CC) $(CFLAGS) -DENABLE_ANIMATION=1 -o $(TARGET_ANIM) $(SRC) $(LDFLAGS)

debug: CFLAGS = -g -O0 -Wall -Wextra -std=c11 -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

run: $(TARGET)
	./$(TARGET) > output.ppm
	@echo "Output written to output.ppm"

animate: $(TARGET_ANIM)
	@echo "Rendering 300-frame animation (10 seconds at 30fps)..."
	./$(TARGET_ANIM)
	@echo "Frames rendered. To create video, run: make video"

video: animate
	@echo "Creating MP4 video from frames..."
	@if command -v ffmpeg >/dev/null 2>&1; then \
		ffmpeg -framerate 30 -i frame_%04d.ppm -c:v libx264 -pix_fmt yuv420p -crf 18 -preset slow output.mp4; \
		echo "Video created: output.mp4"; \
	else \
		echo "FFmpeg not found. Install with: sudo apt install ffmpeg"; \
	fi

clean:
	rm -f $(TARGET) $(TARGET_ANIM) *.ppm *.o frame_*.ppm output.mp4

benchmark: $(TARGET)
	@echo "Running benchmark..."
	@time ./$(TARGET) > /dev/null
