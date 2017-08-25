#include "mini_snprintf.h"
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#define MIN(a, b)       ((a) <= (b) ? (a) : (b))


typedef struct {
    char        digits[32]; // Long enough for any decimal numbers (64-bit)
    size_t      size;
} decimal_t;

static inline void decimal_init(decimal_t* dec) {
    dec->size = 0;
    memset(dec->digits, 0, 32);
}

static inline char* decimal_str(decimal_t* dec) {
    return & dec->digits[32 - dec->size];
}

static inline void decimal_prepand(decimal_t* dec, char c) {
    dec->digits[31 - (dec->size++)] = c;
}

static void decimal_from_ulong(decimal_t* dec, unsigned long u) {
    decimal_init(dec);
    if (u == 0) {
        decimal_prepand(dec, '0');
        return;
    }
    while (u) {
        char digit = '0' + (u % 10);
        decimal_prepand(dec, digit);
        u /= 10UL;
    };
}

static inline unsigned long __abs(long i) {
    unsigned long u;
    if (i >= 0) {
        u = i;
    }
    else {
        u = -(i + 1L) + 1UL; // Be careful with LONG_MIN
    }
    return u;
}

static void decimal_from_long(decimal_t* dec, long i) {
    unsigned long u = __abs(i);
    decimal_from_ulong(dec, u);
    if (i < 0) decimal_prepand(dec, '-');
}

// A formated string consists of one or more directives, each directive is
// either an ordinary string or a conversion specifiation.
typedef enum {
    DT_ORDINARY,
    DT_STRING,
    DT_INT_32,
    DT_INT_64,
    DT_UINT_32,
    DT_UINT_64,
    DT_ESCAPED,
    DT_UNEXPECTED
} directive_type_t;

typedef struct {
    directive_type_t  type;
    size_t            size;
} directive_t;

static int next_directive(const char* fmt, directive_t* directive) {
    // No more directives if EOF
    if (fmt[0] == '\0') return 0;

    // Ordinary
    if (fmt[0] != '%') {
        directive->type = DT_ORDINARY;
        directive->size = 1;
        while (fmt[directive->size] != '\0' && fmt[directive->size] != '%')
            directive->size++;
        return 1;
    }

    // Format string ends prematurely
    if (fmt[1] == '\0') goto on_error;

    // String
    if (fmt[1] == 's') {
        directive->type = DT_STRING;
        directive->size = 2;
        return 1;
    }
    // Escaped
    else if (fmt[1] == '%') {
        directive->type = DT_ESCAPED;
        directive->size = 2;
        return 1;
    }

    // Handle integers
    int is_long = 0;
    if (fmt[1] == 'l') {
        is_long = 1;
    }
    int is_signed;
    switch (fmt[1 + is_long]) {
    case 'd':
        is_signed = 1;
        break;
    case 'u':
        is_signed = 0;
        break;
    default:
        // Format string ends prematurely or unknown/unsupported format
        // specifier
        goto on_error;
    }
    directive->size = is_long ? 3 : 2;
    directive->type = is_long ?
                        (is_signed ?
                            DT_INT_64 :  /* %ld */
                            DT_UINT_64   /* %lu */
                        ) :
                        (is_signed ?
                            DT_INT_32 :  /* %d */
                            DT_UINT_32   /* %u */
                        );
    return 1;
on_error:
    directive->type = DT_UNEXPECTED;
    return 1;
}

#define NULL_STRING                 "(null)"

int mini_snprintf(char* buf, size_t size, const char* fmt, ...) {
    if (size == 0) return 0;
    if (size == 1) { buf[0] = '\0'; return 0; }

    va_list ag;
    va_start(ag, fmt);

    char* str;
    size_t len;

    decimal_t dec;
    long i64;
    unsigned long u64;

    char* buf_pos = buf;
    size_t buf_remain = size - 1; // save one last byte for null
    char* fmt_pos = (char*) fmt;
    directive_t directive;
    while (next_directive(fmt_pos, &directive)) {
        switch(directive.type) {
        case DT_ORDINARY: // a sequence of ordinary chars (not %)
            str = fmt_pos;
            len = directive.size;
            break;
        case DT_STRING: // %s
            str = (char*) va_arg(ag, const char*);
            if (str == NULL) str = NULL_STRING;
            len = strlen(str);
            break;
        case DT_ESCAPED: // %%
            str = "%";
            len = 1;
            break;
        case DT_INT_32: // %d
            i64 = va_arg(ag, int);
            goto signed_int;
        case DT_INT_64: // %ld
            i64 = va_arg(ag, long);
signed_int:
            decimal_from_long(&dec, i64);
            str = decimal_str(&dec);
            len = dec.size;
            break;
        case DT_UINT_32: // %u
            u64 = va_arg(ag, unsigned int);
            goto unsigned_int;
        case DT_UINT_64: // %lu
            u64 = va_arg(ag, unsigned long);
unsigned_int:
            decimal_from_ulong(&dec, u64);
            str = decimal_str(&dec);
            len = dec.size;
            break;
        default: // unexpected
            goto on_error;
        }

        len = MIN(len, buf_remain);
        memcpy(buf_pos, str, len);

        buf_pos += len;
        buf_remain -= len;
        fmt_pos += directive.size;
    }

    va_end(ag);

    // Always null-terminated
    *buf_pos = '\0';
    return (int) (buf_pos - buf);
on_error:
    va_end(ag);
    return -1;
}
