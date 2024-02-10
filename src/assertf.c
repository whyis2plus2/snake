#include <assertf.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define STR const char*
#define UINT const unsigned int

// Assert expr with a custom error message on failure, should be called through the `assertf` macro
void _impl_assertf(STR file, UINT line, STR func, STR estr, int expr, STR fmt, ...) {
    if (expr) {return;}

    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[%s:%d] in %s, assertion of \'%s\' failed: ", file, line, func, estr);
    vfprintf(stderr, fmt, args);

    va_end(args);

    abort();
}
