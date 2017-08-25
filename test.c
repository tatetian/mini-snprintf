#include "mini_snprintf.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define my_printf       mini_snprintf

#define DEFINE_TC_PRELUDE(_name, _n, fmt, _expected)        \
int testcase_##_name(void) {                                \
    const char* expected = _expected;                       \
    size_t n = _n;                                          \
    char* real = malloc(n);                                 \
    assert(real != NULL);                                   \

#define DEFINE_TC_PROLOG(_name, _n, fmt, _expected)         \
    printf("expected: %s, real: %s\n", expected, real);     \
    int success = (expected == NULL && len < 0) || (strncmp(real, expected, n) == 0);\
    free(real);                                             \
    return success;                                         \
}

#define DEFINE_TC0(_name, _n, fmt, _expected)               \
    DEFINE_TC_PRELUDE(_name, _n, fmt, _expected)            \
    int len = my_printf(real, n, fmt);                      \
    DEFINE_TC_PROLOG(_name, _n, fmt, _expected)

#define DEFINE_TC1(_name, _n, fmt, arg0, _expected)        \
    DEFINE_TC_PRELUDE(_name, _n, fmt, _expected)           \
    int len = my_printf(real, n, fmt, arg0);                \
    DEFINE_TC_PROLOG(_name, _n, fmt, _expected)

#define DEFINE_TC2(_name, _n, fmt, arg0, arg1, _expected)  \
    DEFINE_TC_PRELUDE(_name, _n, fmt, _expected)           \
    int len = my_printf(real, n, fmt, arg0, arg1);          \
    DEFINE_TC_PROLOG(_name, _n, fmt, _expected)

#define DEFINE_TC3(_name, _n, fmt, arg0, arg1, arg2, _expected)  \
    DEFINE_TC_PRELUDE(_name, _n, fmt, _expected)           \
    int len = my_printf(real, n, fmt, arg0, arg1, arg2);   \
    DEFINE_TC_PROLOG(_name, _n, fmt, _expected)

#define STRINGIFY(x) #x

#define RUN_TC(name)  do {                                  \
    printf("\nRun test case \"%s\"\n", STRINGIFY(name));    \
    int success = testcase_##name();                        \
    if (success) printf("Passed ^_^\n");                    \
    else printf("Failed -_-\n");                            \
} while (0)

// Test cases
DEFINE_TC0(plain, 128, "Hello World",
           "Hello World")
DEFINE_TC1(string, 128, "This string is %s", "Hello Kitty",
           "This string is Hello Kitty")
DEFINE_TC3(math, 128, "%d + %d = %d!", 1, 2, (1+2),
           "1 + 2 = 3!")
DEFINE_TC2(int, 128, "int: min = %d, max = %d", INT_MIN, INT_MAX,
           "int: min = -2147483648, max = 2147483647")
DEFINE_TC2(uint, 128, "unsigned int: min = %u, max = %u", 0U, UINT_MAX,
           "unsigned int: min = 0, max = 4294967295")
DEFINE_TC2(long, 128, "long: min = %ld, max = %ld", LONG_MIN, LONG_MAX,
           "long: min = -9223372036854775808, max = 9223372036854775807")
DEFINE_TC2(ulong, 128, "unsigned long: min = %lu, max = %lu", 0UL, ULONG_MAX,
           "unsigned long: min = 0, max = 18446744073709551615")
DEFINE_TC0(special_chars, 128, "Special chars: \\, \", \', %%",
          "Special chars: \\, \", \', %")
DEFINE_TC0(overflow, 29, "Dots should NOT be displayed........",
          "Dots should NOT be displayed")
DEFINE_TC0(malformed, 128, "Unknown specifier %q",
          NULL)

int main() {
    RUN_TC(plain);
    RUN_TC(string);
    RUN_TC(math);
    RUN_TC(int);
    RUN_TC(uint);
    RUN_TC(long);
    RUN_TC(ulong);
    RUN_TC(special_chars);
    RUN_TC(overflow);
    RUN_TC(malformed);
    return 0;
}
