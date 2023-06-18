cmake_minimum_required(VERSION 3.8)

project(libLVC_preprocess VERSION "0.0.0" LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" OFF)

set(LVC_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

set(libLVC_CONFIG_DIR "${PROJECT_SOURCE_DIR}/include/LVC/common")
configure_file("${libLVC_CONFIG_DIR}/cmake.h.in" "${libLVC_CONFIG_DIR}/cmake.h")

add_subdirectory("src")
add_subdirectory("apps")
