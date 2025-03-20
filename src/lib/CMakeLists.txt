add_library(mca-lib INTERFACE)

target_compile_features(mca-lib INTERFACE cxx_std_20)
target_include_directories(mca-lib INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
\        $<BUILD_INTERFACE:${OpenCV_INCLUDE_DIRS}>
)
target_link_libraries(mca-lib INTERFACE
        ${OpenCV_LIBS}
        argparse
        tlct::lib::static
)

if (MSVC)
    target_compile_options(mca-lib INTERFACE /arch:AVX2)
else ()
    target_compile_options(mca-lib INTERFACE -mavx2)
endif ()

if (MSVC)
    target_compile_definitions(mca-lib INTERFACE "_CRT_SECURE_NO_WARNINGS")
endif ()

add_library(mca::lib ALIAS mca-lib)
