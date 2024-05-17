include(CMakeDependentOption)

option(BUILD_TESTS "Build tests" OFF)
option(MCA_HEADER_ONLY "Enable the header-only mode" OFF)
option(BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" ON)
cmake_dependent_option(MCA_BUILD_SHARED_LIBS "Specifies the type of MCA to build" ON
        "BUILD_SHARED_LIBS;NOT MCA_HEADER_ONLY" OFF)

if (MSVC)
    option(MCA_WITH_STATIC_CRT "Link with STATIC CRT (this wont work if OpenCV is already linked with SHARED CRT)" OFF)
    set(MSVC_RUNTIME_TYPE MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${MCA_WITH_STATIC_CRT}>>:DLL>)
endif ()

set(MCA_CONFIG_DIR "${CMAKE_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIG_DIR}/cmake.h.in" "${MCA_CONFIG_DIR}/cmake.h")