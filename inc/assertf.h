#ifndef _assertf_h
#define _assertf_h 1

#include "SDL2/SDL_stdinc.h"

#define _ASSERTF_ESTR(_expr, _estr, ...) _impl_assertf(__FILE__, __LINE__, __func__, _estr, !!(_expr), __VA_ARGS__)

#ifdef ASSERTF_USE_CUSTOM_ASSERT
#define assert(_expr) _ASSERTF_ESTR(_expr, #_expr, "\b\b.\n")

#ifndef static_assert
#define static_assert(_expr, _msg) _Static_assert(_expr, _msg)
#endif
#endif

#define STR const char*
#define UINT unsigned int

// Assert _expr with a custom message on failure
#define assertf(_expr, ...) _ASSERTF_ESTR(_expr, #_expr, __VA_ARGS__)

// Assert expr with a custom error message on failure, should be called through the `assertf` macro
void _impl_assertf(STR file, UINT line, STR func, STR estr, int expr, SDL_PRINTF_FORMAT_STRING STR fmt, ...) 
     SDL_PRINTF_VARARG_FUNC(6);

#undef STR
#undef UINT

#endif // !_assertf_h
