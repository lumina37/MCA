#pragma once

#include "mca/common/cmake.h"

#if defined(_WIN32) && defined(LVC_BUILD_SHARED_LIBS)
#ifdef _LVC_EXPORTS
#define LVC_EXPORT __declspec(dllexport)
#else
#define LVC_EXPORT __declspec(dllimport)
#endif
#else
#ifdef _LVC_EXPORTS
#define LVC_EXPORT __attribute__((visibility("default")))
#else
#define LVC_EXPORT
#endif
#endif

#ifdef __GNUC__
#define LVC_CONSTFUNC __attribute__((const))
#define LVC_PUREFUNC __attribute__((pure))
#else
#define LVC_CONSTFUNC
#define LVC_PUREFUNC
#endif
