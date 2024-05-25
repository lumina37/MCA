#pragma once

#include "cmake.h"

#ifdef MCA_HEADER_ONLY
#    define MCA_API
#else
#    ifdef MCA_BUILD_SHARED_LIBS
#        ifdef _MSC_VER
#            ifdef _MCA_BUILD_LIBS
#                define MCA_API __declspec(dllexport)
#            else
#                define MCA_API __declspec(dllimport)
#            endif
#        else
#            ifdef _MCA_BUILD_LIBS
#                define MCA_API __attribute__((visibility("default")))
#            else
#                define MCA_API
#            endif
#        endif
#    else
#        define MCA_API
#    endif
#endif
