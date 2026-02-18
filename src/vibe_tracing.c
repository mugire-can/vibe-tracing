#include "vibe_tracing.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

static FILE *trace_output = NULL;
static trace_level_t min_trace_level = TRACE_LEVEL_DEBUG;
static pthread_mutex_t trace_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Level strings for output */
static const char *level_strings[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

int vibe_trace_init(const char *output_file) {
    pthread_mutex_lock(&trace_mutex);
    
    if (trace_output != NULL && trace_output != stdout && trace_output != stderr) {
        fclose(trace_output);
    }
    
    if (output_file == NULL || strcmp(output_file, "stdout") == 0) {
        trace_output = stdout;
    } else if (strcmp(output_file, "stderr") == 0) {
        trace_output = stderr;
    } else {
        trace_output = fopen(output_file, "a");
        if (trace_output == NULL) {
            pthread_mutex_unlock(&trace_mutex);
            return -1;
        }
    }
    
    pthread_mutex_unlock(&trace_mutex);
    return 0;
}

void vibe_trace_cleanup(void) {
    pthread_mutex_lock(&trace_mutex);
    
    if (trace_output != NULL && trace_output != stdout && trace_output != stderr) {
        fclose(trace_output);
        trace_output = NULL;
    }
    
    pthread_mutex_unlock(&trace_mutex);
}

void vibe_trace_set_level(trace_level_t level) {
    pthread_mutex_lock(&trace_mutex);
    min_trace_level = level;
    pthread_mutex_unlock(&trace_mutex);
}

void vibe_trace_log(trace_level_t level, const char *file, int line,
                    const char *function, const char *format, ...) {
    pthread_mutex_lock(&trace_mutex);
    
    /* Validate trace level */
    if (level < TRACE_LEVEL_DEBUG || level > TRACE_LEVEL_ERROR) {
        pthread_mutex_unlock(&trace_mutex);
        return;
    }
    
    /* Check if this level should be logged */
    if (level < min_trace_level) {
        pthread_mutex_unlock(&trace_mutex);
        return;
    }
    
    /* If no output is set, default to stdout */
    if (trace_output == NULL) {
        trace_output = stdout;
    }
    
    /* Get timestamp */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    /* Format timestamp using thread-safe localtime_r */
    time_t seconds = ts.tv_sec;
    struct tm tm_info;
    localtime_r(&seconds, &tm_info);
    char time_buffer[64];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
    
    /* Print trace header */
    fprintf(trace_output, "[%s.%06ld] [%s] %s:%d (%s): ",
            time_buffer, ts.tv_nsec / 1000,
            level_strings[level], file, line, function);
    
    /* Print the message */
    va_list args;
    va_start(args, format);
    vfprintf(trace_output, format, args);
    va_end(args);
    
    fprintf(trace_output, "\n");
    fflush(trace_output);
    
    pthread_mutex_unlock(&trace_mutex);
}
