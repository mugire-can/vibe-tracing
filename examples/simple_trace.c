#include "vibe_tracing.h"
#include <unistd.h>

void example_function(int value) {
    TRACE_DEBUG("Processing value: %d", value);
    
    if (value < 0) {
        TRACE_WARN("Negative value detected: %d", value);
    }
    
    if (value > 100) {
        TRACE_ERROR("Value exceeds maximum: %d", value);
    } else {
        TRACE_INFO("Value within acceptable range: %d", value);
    }
}

int main(void) {
    /* Initialize tracing to stdout */
    if (vibe_trace_init("stdout") != 0) {
        fprintf(stderr, "Failed to initialize tracing\n");
        return 1;
    }
    
    TRACE_INFO("Vibe Tracing Example Started");
    
    /* Test with various values */
    TRACE_DEBUG("Testing with different values...");
    
    example_function(50);
    example_function(-10);
    example_function(150);
    example_function(75);
    
    /* Test different trace levels */
    TRACE_INFO("Changing trace level to INFO");
    vibe_trace_set_level(TRACE_LEVEL_INFO);
    
    TRACE_DEBUG("This debug message should not appear");
    TRACE_INFO("This info message should appear");
    TRACE_WARN("This is a warning");
    TRACE_ERROR("This is an error");
    
    /* Cleanup */
    TRACE_INFO("Vibe Tracing Example Completed");
    vibe_trace_cleanup();
    
    return 0;
}
