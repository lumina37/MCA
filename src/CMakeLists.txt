find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)
find_package(Boost REQUIRED)

set(libLVC_preprocess_SRCS "preprocess/forward.cpp" "preprocess/backward.cpp" "preprocess/debug.cpp")
set(common_SRCS "common/config.cpp" "common/config_reader.cpp")

if (BUILD_SHARED_LIBS)
    add_library(libLVC_preprocess SHARED ${libLVC_preprocess_SRCS} ${common_SRCS})
    target_compile_definitions(libLVC_preprocess PRIVATE "_LVC_EXPORTS")
else ()
    add_library(libLVC_preprocess STATIC ${libLVC_preprocess_SRCS} ${common_SRCS})
endif ()

target_include_directories(libLVC_preprocess PUBLIC "${PROJECT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(libLVC_preprocess PUBLIC ${OpenCV_LIBS} ${Boost_LIBRARIES})
target_compile_features(libLVC_preprocess PUBLIC cxx_std_11)
