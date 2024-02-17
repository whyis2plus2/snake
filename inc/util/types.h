#ifndef _types_h
#define _types_h 1

#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <stdbool.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

#define SIGN(_x) (((_x) < 0) ? -1 : ((_x) > 0) ? 1 : 0)
#define ABS(_x) ((_x) * SIGN(_x))
#define MIN(_x, _y) (((_x) < (_y)) ? (_x) : (_y))
#define MAX(_x, _y) (((_x) > (_y)) ? (_x) : (_y))
#define CLAMP(_x, _min, _max) (MAX((_min), MIN((_max), (_x))))

inline bool f32_eq(f32 x, f32 y) {
    // handle infinities
    if (x == y) {return true;}

    return ABS(x - y) < (ABS(x) + ABS(y)) * FLT_EPSILON;
}

inline bool f64_eq(f64 x, f64 y) {
    // handle infinities
    if (x == y) {return true;}
    
    return ABS(x - y) < (ABS(x) + ABS(y)) * DBL_EPSILON;
}

static_assert(
    FLT_MANT_DIG == 24   && \
    FLT_MAX_EXP == 128   && \
    FLT_MIN_EXP == -125  && \
    DBL_MANT_DIG == 53   && \
    DBL_MAX_EXP == 1024  && \
    DBL_MIN_EXP == -1021,
    "system does not use IEEE-754 floats"
);

typedef struct {f32 x, y;} v2f;
typedef struct {i32 x, y;} v2i;

#endif // !_types_h
