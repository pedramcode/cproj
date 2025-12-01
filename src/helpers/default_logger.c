#include "helpers/default_logger.h"
#include "helpers/time.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void default_logger(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        return;
    }

    char* content = malloc((size_t)size + 1);
    if (!content) {
        va_end(args);
        return;
    }

    vsnprintf(content, size + 1, fmt, args);

    va_end(args);

    fprintf(stdout, "(LOG) [%s] %s\n", time_now_string(), content);
    fflush(stdout);

    free(content);
}
