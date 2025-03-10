cmake_minimum_required(VERSION 3.24)

# The pre-defined value will not be replaced by default value from `option`.
cmake_policy(SET CMP0077 NEW)
# Enable target property setting `MSVC_RUNTIME_LIBRARY`.
cmake_policy(SET CMP0091 NEW)

project(mca
        VERSION 0.4.0.6
        LANGUAGES CXX
)

# Compile Options
include(CMakeDependentOption)

cmake_dependent_option(MCA_ENABLE_LTO "Enable full link-time-optimizations (LTO)" OFF
        "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

if (MSVC)
    option(MCA_WITH_STATIC_CRT "Link with STATIC CRT" OFF)
    set(MSVC_RUNTIME_TYPE MultiThreaded$<$<NOT:$<BOOL:${MCA_WITH_STATIC_CRT}>>:DLL>)
endif ()

# Dependencies
include(FetchContent)

find_package(OpenCV REQUIRED imgproc)

set(MCA_TLCT_PATH "https://github.com/lumina37/TLCT/archive/refs/tags/v1.4.1.0.tar.gz" CACHE STRING
        "Specifies the path of TLCT")

FetchContent_Declare(
        tlct
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL ${MCA_TLCT_PATH}
)
FetchContent_MakeAvailable(tlct)

set(MCA_ARGPARSE_PATH "https://github.com/p-ranav/argparse/archive/refs/tags/v3.2.tar.gz" CACHE STRING
        "Specifies the path of argparse")
set(TLCT_ARGPARSE_PATH ${MCA_ARGPARSE_PATH} CACHE STRING "")
FetchContent_Declare(
        argparse
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL ${MCA_ARGPARSE_PATH}
)
FetchContent_MakeAvailable(argparse)

# Helper
function(copy_dlls_if_needed name)
    if (WIN32)
        add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                copy_if_different $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE:${name}> $<TARGET_FILE_DIR:${name}> COMMAND_EXPAND_LISTS)
    endif ()
endfunction()

set(MCA_COMPILE_INFO "[MCA v${PROJECT_VERSION}] [OpenCV v${OpenCV_VERSION}] by [${CMAKE_CXX_COMPILER_ID} v${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_SYSTEM_PROCESSOR})]")

set(MCA_CONFIGURE_DIR "${PROJECT_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIGURE_DIR}/cmake.h.in" "${MCA_CONFIGURE_DIR}/cmake.h" @ONLY)


add_subdirectory(src)
