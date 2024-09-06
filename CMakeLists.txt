cmake_minimum_required(VERSION 3.24)

include(cmake/policies.cmake)

project(MCA VERSION 0.3.1 LANGUAGES CXX)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/helper.cmake)

add_subdirectory(src)
