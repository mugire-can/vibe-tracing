#ifndef VIBE_TRACING_H
#define VIBE_TRACING_H

#include <stdio.h>
#include <stdint.h>
#include <time.h>

/* Trace levels */
typedef enum {
    TRACE_LEVEL_DEBUG = 0,
    TRACE_LEVEL_INFO,
    TRACE_LEVEL_WARN,
    TRACE_LEVEL_ERROR
} trace_level_t;

/* Initialize the tracing system */
int vibe_trace_init(const char *output_file);

/* Cleanup the tracing system */
void vibe_trace_cleanup(void);

/* Set the minimum trace level */
void vibe_trace_set_level(trace_level_t level);

/* Log a trace event */
void vibe_trace_log(trace_level_t level, const char *file, int line, 
                    const char *function, const char *format, ...);

/* Convenience macros */
#define TRACE_DEBUG(fmt, ...) \
    vibe_trace_log(TRACE_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define TRACE_INFO(fmt, ...) \
    vibe_trace_log(TRACE_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define TRACE_WARN(fmt, ...) \
    vibe_trace_log(TRACE_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define TRACE_ERROR(fmt, ...) \
    vibe_trace_log(TRACE_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#endif /* VIBE_TRACING_H */
