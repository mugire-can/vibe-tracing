CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -Werror -std=c11 -O2
AR      ?= ar

INCDIR  = include
SRCDIR  = src
OBJDIR  = build
LIBDIR  = build
EXDIR   = examples

CFLAGS += -I$(INCDIR)

# Library
LIB_SRC  = $(SRCDIR)/pdf_loader.c
LIB_OBJ  = $(OBJDIR)/pdf_loader.o
LIB_NAME = $(LIBDIR)/libpdfloader.a

# Example
EX_SRC   = $(EXDIR)/load_pdf.c
EX_BIN   = $(OBJDIR)/load_pdf

# Test
TEST_SRC = tests/test_pdf_loader.c
TEST_BIN = $(OBJDIR)/test_pdf_loader

.PHONY: all lib examples test clean

all: lib examples

lib: $(LIB_NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIB_OBJ): $(LIB_SRC) $(INCDIR)/pdf_loader.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_NAME): $(LIB_OBJ)
	$(AR) rcs $@ $^

examples: $(EX_BIN)

$(EX_BIN): $(EX_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) -lpdfloader -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(LIB_NAME) | $(OBJDIR)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) -lpdfloader -o $@

clean:
	rm -rf $(OBJDIR)
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

