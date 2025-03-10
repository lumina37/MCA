add_library(mca-lib INTERFACE)
add_library(mca::lib ALIAS mca-lib)
target_compile_features(mca-lib INTERFACE cxx_std_20)
target_include_directories(mca-lib INTERFACE
        ${OpenCV_INCLUDE_DIR}
        "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(mca-lib INTERFACE
        ${OpenCV_LIBS}
        argparse
        tlct::lib::header-only
)
