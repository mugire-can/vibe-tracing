CC = gcc
CFLAGS = -Wall -Wextra -O3 -march=native -ffast-math -pthread -std=c11
LDFLAGS = -lm -lpthread
TARGET = raytracer
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run benchmark

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) > output.ppm
	@echo "Image saved to output.ppm"

benchmark: $(TARGET)
	@echo "Running benchmark..."
	@time ./$(TARGET) > /dev/null

clean:
	rm -f $(TARGET) $(OBJECTS) output.ppm

# Debug build
debug: CFLAGS = -Wall -Wextra -g -pthread -std=c11
debug: clean $(TARGET)
