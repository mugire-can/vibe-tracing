# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude -pthread
LDFLAGS = -pthread

# Directories
SRC_DIR = src
INC_DIR = include
EXAMPLES_DIR = examples
BUILD_DIR = build

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Library name
LIBRARY = $(BUILD_DIR)/libvibe_tracing.a

# Examples
EXAMPLES = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(EXAMPLES:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/%)

# Default target
all: $(LIBRARY) $(EXAMPLE_BINS)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^
	@echo "Library created: $(LIBRARY)"

# Build examples
$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIBRARY) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lvibe_tracing $(LDFLAGS) -o $@
	@echo "Example built: $@"

# Run the simple example
run: $(BUILD_DIR)/simple_trace
	@echo "Running simple_trace example:"
	@$(BUILD_DIR)/simple_trace

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean run

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build library and examples (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Run the simple trace example"
	@echo "  help    - Show this help message"
