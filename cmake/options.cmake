option(BUILD_TESTS "Build tests" OFF)
option(BUILD_SHARED_LIBS "Specifies the type of libraries (SHARED or STATIC) to build" OFF)

if (MSVC AND (NOT OpenCV_SHARED))
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

set(LVC_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

set(MCA_CONFIG_DIR "${CMAKE_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIG_DIR}/cmake.h.in" "${MCA_CONFIG_DIR}/cmake.h")
