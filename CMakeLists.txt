cmake_minimum_required(VERSION 3.15)

# The pre-defined value will not be replaced by default value from `option`.
cmake_policy(SET CMP0077 NEW)
# Enable target property setting `MSVC_RUNTIME_LIBRARY`.
cmake_policy(SET CMP0091 NEW)

project(mca
        VERSION 0.4.2
        LANGUAGES CXX
)

set(MCA_VERSION_BETA_SUFFIX "alpha.0")
if (NOT MCA_VERSION_BETA_SUFFIX)
    set(MCA_VERSION ${PROJECT_VERSION})
else ()
    set(MCA_VERSION ${PROJECT_VERSION}-${MCA_VERSION_BETA_SUFFIX})
endif ()

# Compile Options
include(CMakeDependentOption)

cmake_dependent_option(MCA_ENABLE_LTO "Enable full link-time-optimizations (LTO)" OFF
        "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

if (MSVC)
    option(MCA_WITH_STATIC_CRT "Link with STATIC CRT" OFF)
    set(_MCA_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${TLCT_WITH_STATIC_CRT}>>:DLL>)
endif ()

# Dependencies
include(FetchContent)

find_package(OpenCV REQUIRED imgproc)

set(MCA_TLCT_PATH "https://github.com/lumina37/TLCT/archive/refs/tags/v1.4.5.tar.gz" CACHE STRING
        "Specifies the path of TLCT")

FetchContent_Declare(
        tlct
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL ${MCA_TLCT_PATH}
)
FetchContent_MakeAvailable(tlct)

if (DEFINED PROJECT_NAME)
    set(MCA_ARGPARSE_PATH "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.tar.gz" CACHE STRING
            "Specifies the path of argparse")
    set(TLCT_ARGPARSE_PATH ${MCA_ARGPARSE_PATH} CACHE STRING "")
    FetchContent_Declare(
            argparse
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            URL ${MCA_ARGPARSE_PATH}
    )
    FetchContent_MakeAvailable(argparse)
endif ()

# Helper
set(MCA_COMPILE_INFO "[MCA v${MCA_VERSION}] [OpenCV v${OpenCV_VERSION}] by [${CMAKE_CXX_COMPILER_ID} v${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_SYSTEM_PROCESSOR})]")

set(MCA_CONFIGURE_DIR "${PROJECT_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIGURE_DIR}/config.h.in" "${MCA_CONFIGURE_DIR}/config.h" @ONLY)


add_subdirectory(src)
