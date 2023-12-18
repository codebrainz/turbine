#pragma once

#include <cassert>
#include <utility>

#define M_UNUSED(x) ((void)(x))

#ifdef __GNUC__
#define M_LIKELY(x) __builtin_expect(!!(x), 1)
#define M_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define M_LIKELY(x) (x)
#define M_UNLIKELY(x) (x)
#endif

#if defined(__cpp_lib_unreachable) && __cplusplus >= 202202L
#define M_UNREACHABLE() std::unreachable()
#elif defined(__GNUC__)
#define M_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define M_UNREACHABLE() __assume(false)
#else
#define M_UNREACHABLE() assert(false && "unreachable")
#endif
