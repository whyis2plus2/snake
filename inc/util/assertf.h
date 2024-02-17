#ifndef _ASSERTF_h
#define _ASSERTF_h 1

#include <stdio.h>
#include <stdlib.h>   

#define ASSERTF(_expr, ...) \
    if (!(_expr)) { \
        fprintf(stderr, "[%s:%d] assertion of '%s' failed: ", __FILE__, __LINE__, #_expr); \
        fprintf(stderr, __VA_ARGS__); \
        abort(); \
    }


// same as assertf, but don't print line or file info
#define ASSERTF_2(_expr, ...) \
    if (!(_expr)) { \
        fprintf(stderr, "assertion of '%s' failed: ", #_expr); \
        fprintf(stderr, __VA_ARGS__); \
        abort(); \
    }

#endif // !_ASSERTF_h
