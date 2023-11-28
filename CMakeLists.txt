cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0091 NEW)  # NO MORE WINDOWS PLZ!

project(libMCA VERSION "0.1.0" LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" OFF)

set(LVC_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

set(libMCA_CONFIG_DIR "${PROJECT_SOURCE_DIR}/include/MCA/common")
configure_file("${libMCA_CONFIG_DIR}/cmake.h.in" "${libMCA_CONFIG_DIR}/cmake.h")

add_subdirectory("src")
add_subdirectory("apps")
add_subdirectory("tests")
