cmake_minimum_required(VERSION 3.21)
include(cmake/policies.cmake)

project(MCA VERSION 0.2.6 LANGUAGES CXX)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/helper.cmake)

add_subdirectory(src)
if (MCA_BUILD_TESTS)
    add_subdirectory(tests)
endif ()
