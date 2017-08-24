#include "mini_snprintf.h"
#include <string.h>
#include <stdarg.h>

#define MIN(a, b)       ((a) <= (b) ? (a) : (b))

static inline size_t __puts(char* buf, size_t bufsize, char* str, size_t strlen) {
    strlen = MIN(strlen, bufsize);
    memcpy(buf, str, strlen);
    return strlen;
}

int mini_snprintf(char* buf, size_t size, const char* fmt, ...) {
    return 0;
}
