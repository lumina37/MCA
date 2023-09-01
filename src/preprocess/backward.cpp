#pragma once

#ifdef LVC_USE_LEGACY
#include "backward_v1.cc"
#else
#include "backward_v2.cc"
#endif