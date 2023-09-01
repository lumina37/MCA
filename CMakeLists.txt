cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0091 NEW)  # NO MORE WINDOWS PLZ!

project(libLVC_preprocess VERSION "0.1.0" LANGUAGES CXX)

if(MSVC AND (NOT OpenCV_SHARED))
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" OFF)

set(LVC_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

set(libLVC_CONFIG_DIR "${PROJECT_SOURCE_DIR}/include/LVC/common")
configure_file("${libLVC_CONFIG_DIR}/cmake.h.in" "${libLVC_CONFIG_DIR}/cmake.h")

add_subdirectory("src")
add_subdirectory("apps")
add_subdirectory("tests")
