CC = gcc
CFLAGS = -O3 -march=native -ffast-math -Wall -Wextra -std=c11
LDFLAGS = -lpthread -lm

TARGET = raytracer
SRC = main.c

.PHONY: all clean run debug benchmark

all: $(TARGET)

$(TARGET): $(SRC) vec3.h ray.h color.h camera.h material.h sphere.h plane.h triangle.h aabb.h scene.h texture.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

debug: CFLAGS = -g -O0 -Wall -Wextra -std=c11 -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

run: $(TARGET)
	./$(TARGET) > output.ppm
	@echo "Output written to output.ppm"

benchmark: $(TARGET)
	@echo "Running benchmark..."
	@time ./$(TARGET) > /dev/null

clean:
	rm -f $(TARGET) *.ppm *.o
