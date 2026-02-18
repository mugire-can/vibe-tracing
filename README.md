# vibe-tracing

A lightweight, thread-safe tracing library for C applications.

## Features

- **Thread-safe logging**: Uses mutex locks to ensure thread-safe operation
- **Multiple trace levels**: DEBUG, INFO, WARN, ERROR
- **Flexible output**: Write to stdout, stderr, or custom files
- **Timestamped logs**: Each trace includes microsecond-precision timestamps
- **Source location**: Automatic capture of file, line, and function information
- **Easy to use**: Simple macro-based API

## Building

Build the library and examples:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

Run the example:

```bash
make run
```

## Usage

### Basic Example

```c
#include "vibe_tracing.h"

int main() {
    // Initialize tracing (to stdout)
    vibe_trace_init("stdout");
    
    // Log messages at different levels
    TRACE_DEBUG("This is a debug message");
    TRACE_INFO("Application started");
    TRACE_WARN("This is a warning");
    TRACE_ERROR("This is an error");
    
    // Clean up
    vibe_trace_cleanup();
    return 0;
}
```

### Setting Trace Level

```c
// Only show INFO and above (filters out DEBUG)
vibe_trace_set_level(TRACE_LEVEL_INFO);
```

### Output to File

```c
// Initialize with file output
vibe_trace_init("/tmp/app.log");
```

## API Reference

### Initialization and Cleanup

- `int vibe_trace_init(const char *output_file)` - Initialize tracing system
  - Returns 0 on success, -1 on failure
  - `output_file` can be "stdout", "stderr", or a file path
  
- `void vibe_trace_cleanup(void)` - Cleanup and close trace output

### Configuration

- `void vibe_trace_set_level(trace_level_t level)` - Set minimum trace level

### Logging Macros

- `TRACE_DEBUG(format, ...)` - Log debug message
- `TRACE_INFO(format, ...)` - Log info message
- `TRACE_WARN(format, ...)` - Log warning message
- `TRACE_ERROR(format, ...)` - Log error message

## Project Structure

```
vibe-tracing/
├── include/          # Public header files
│   └── vibe_tracing.h
├── src/              # Implementation files
│   └── vibe_tracing.c
├── examples/         # Example programs
│   └── simple_trace.c
├── build/            # Build output (generated)
├── Makefile          # Build configuration
└── README.md         # This file
```

## License

This is a demonstration project.