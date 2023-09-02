set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)

if (MSVC AND (NOT OpenCV_SHARED))
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

set(libLVC_preprocess_SRCS "preprocess/forward.cpp" "preprocess/backward.cpp" "preprocess/debug.cpp")
set(common_SRCS "config/config.cpp" "config/parser.cpp")

set(pugixml_SRC_DIR "${PROJECT_SOURCE_DIR}/thirdparty/pugixml/src")
set(pugixml_SRCS "${pugixml_SRC_DIR}/pugixml.cpp")

function(lvc_lib_linkage tg)
    if (BUILD_SHARED_LIBS)
        add_library(${tg} SHARED ${libLVC_preprocess_SRCS} ${common_SRCS} ${pugixml_SRCS})
        target_compile_definitions(${tg} PRIVATE "_LVC_EXPORTS")
    else ()
        add_library(${tg} STATIC ${libLVC_preprocess_SRCS} ${common_SRCS} ${pugixml_SRCS})
    endif ()

    target_include_directories(${tg} PUBLIC "${PROJECT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS} ${pugixml_SRC_DIR})
    target_link_libraries(${tg} PUBLIC ${OpenCV_LIBS})
    target_compile_features(${tg} PUBLIC cxx_std_11)
endfunction()

lvc_lib_linkage(libLVC_preprocess)
lvc_lib_linkage(libLVC_preprocess_legacy)
target_compile_definitions(libLVC_preprocess_legacy PUBLIC LVC_USE_LEGACY)
