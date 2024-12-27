set(MCA_LIB_NAME mca-lib)
add_library(${MCA_LIB_NAME} INTERFACE)
set_target_properties(${MCA_LIB_NAME} PROPERTIES
        VERSION ${mca_VERSION}
        SOVERSION ${mca_VERSION}
        DEBUG_POSTFIX "-${mca_VERSION}d"
        RELEASE_POSTFIX "-${mca_VERSION}"
)
target_compile_features(${MCA_LIB_NAME} INTERFACE cxx_std_20)
target_include_directories(${MCA_LIB_NAME} INTERFACE
        ${OpenCV_INCLUDE_DIR}
        "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(${MCA_LIB_NAME} INTERFACE
        ${OpenCV_LIBS}
        argparse
        tlct::lib
)
