find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)

set(libLVC_preprocess_SRCS "preprocess/forward.cpp" "preprocess/backward.cpp" "preprocess/debug.cpp")
set(common_SRCS "config/config.cpp" "config/parser.cpp")

set(pugixml_SRC_DIR "${PROJECT_SOURCE_DIR}/thirdparty/pugixml/src")
set(pugixml_SRCS "${pugixml_SRC_DIR}/pugixml.cpp")

if (BUILD_SHARED_LIBS)
    add_library(libLVC_preprocess SHARED ${libLVC_preprocess_SRCS} ${common_SRCS} ${pugixml_SRCS})
    target_compile_definitions(libLVC_preprocess PRIVATE "_LVC_EXPORTS")
else ()
    add_library(libLVC_preprocess STATIC ${libLVC_preprocess_SRCS} ${common_SRCS} ${pugixml_SRCS})
endif ()

target_include_directories(libLVC_preprocess PUBLIC "${PROJECT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS} ${pugixml_SRC_DIR})
target_link_libraries(libLVC_preprocess PUBLIC ${OpenCV_LIBS})
target_compile_features(libLVC_preprocess PUBLIC cxx_std_11)
target_compile_definitions(libLVC_preprocess PUBLIC LVC_USE_LEGACY)
