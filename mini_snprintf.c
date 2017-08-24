#include "mini_snprintf.h"
#include <string.h>
#include <stdarg.h>

#define MIN(a, b)       ((a) <= (b) ? (a) : (b))


static size_t utoa(unsigned long i, char* a) {
    size_t len = 0;
    return len;
}

static size_t itoa(long i, char* a) {
    size_t len = 0;
    if (i < 0) {
        *(a++) = '-';
        i *= -1;
        len = 1;
    }
    len += utoa((unsigned long)i, a);
    return len;
}

typedef enum {
    DT_ORDINARY,
    DT_STRING,
    DT_INT_32,
    DT_INT_64,
    DT_UINT_32,
    DT_UINT_64,
    DT_UNEXPECTED
} directive_type_t;

typedef struct {
    directive_type_t  type;
    size_t            size;
} directive_t;

static int next_directive(const char* fmt, directive_t* directive) {
    if 
}

int mini_snprintf(char* buf, size_t size, const char* fmt, ...) {
    if (size == 0) return 0;
    if (size == 1) { buf[0] = '\0'; return 0; }

    va_list ag;
    va_start(ag, fmt);

    char digits[32]; // Big enough for any 64-bit integers in decimal
    long signed_int;
    unsigned long unsigned_int;

    directive_t directive;
    char* str;
    size_t len;

    size_t buf_remain = size - 1; // save one byte for null
    char* fmt_pos = (char*) fmt;
    char* fmt_end = fmt + strlen(fmt);
    while (next_directive(fmt_pos, &directive)) {
        switch(directive.type) {
        case ordinary: // a sequence of ordinary chars (not %)
            str = fmt_pos;
            len = directive.size;
            break;
        case string: // %s
            str = (char*) va_arg(ag, const char*);
            if (part == NULL) part = NULL_STRING;
            len = strlen(part);
            break;
        case escaped: // %%
            str = '%';
            len = 1;
            break;
        case i32: // %d
            signed_int = va_arg(ag, int);
        case i64: // %ld
            signed_int = va_arg(ag, long);
            len = itoa(signed_int, digits)
            str = digits;
            break;
        case u32: // %u
            unsigined_int = va_arg(ag, unsigned int);
        case u64: // %lu
            unsigined_int = va_arg(ag, unsigned long);
            len = utoa(unsigned_int, digits)
            str = digits;
            break;
        default: // unexpected
            // TODO:va_end
            return -1;
        }

        len = MIN(len, buf_remain);
        memcpy(buf_pos, str, len);

        buf_pos += len;
        fmt_pos += directive.size;
    }

    va_end(ag);

    buf_pos = '\0';
    return buf_pos - buf;
on_error:
    return -1;
}
