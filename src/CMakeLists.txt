find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)

# We do not recommend using GLOB to collect a list of source files from your
# source tree See: https://cmake.org/cmake/help/latest/command/file.html#glob
set(libLVC_preprocess_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS})
set(libLVC_preprocess_SRC_DIR "${PROJECT_SOURCE_DIR}/src/preprocess")
set(libLVC_preprocess_SRCS "${libLVC_preprocess_SRC_DIR}/forward.cpp" "${libLVC_preprocess_SRC_DIR}/backward.cpp" "${libLVC_preprocess_SRC_DIR}/debug.cpp")
set(common_SRC_DIR "${PROJECT_SOURCE_DIR}/src/common")
set(common_SRCS "${common_SRC_DIR}/config.cpp")

# New in 3.1: Source arguments to add_library may use "generator expressions"
# with the syntax $<...>
if (BUILD_SHARED_LIBS)
    add_library(libLVC_preprocess SHARED ${libLVC_preprocess_SRCS} ${common_SRCS})
    target_compile_definitions(libLVC_preprocess PRIVATE "_LVC_EXPORTS")
else ()
    add_library(libLVC_preprocess STATIC ${libLVC_preprocess_SRCS} ${common_SRCS})
endif ()
target_include_directories(libLVC_preprocess PUBLIC ${libLVC_preprocess_INCLUDE_DIRS})
target_link_libraries(libLVC_preprocess PUBLIC ${OpenCV_LIBS})
target_compile_features(libLVC_preprocess PUBLIC cxx_std_11)
