if (MCA_HEADER_ONLY)
    set(MCA_LIB_TYPE "INTERFACE")
    set(MCA_LIB_DEP_SCOPE "INTERFACE")
else ()
    if (MCA_BUILD_SHARED_LIBS)
        set(MCA_LIB_TYPE "SHARED")
    else ()
        set(MCA_LIB_TYPE "STATIC")
    endif ()
    set(MCA_LIB_DEP_SCOPE "PUBLIC")

    set(MCA_LIB_SRCS "lib.cpp")
    if (PUGIXML_HEADER_ONLY)
        list(APPEND MCA_LIB_SRCS "${pugixml_SOURCE_DIR}/src/pugixml.cpp")
    endif ()
endif ()

set(MCA_LIB_NAME mca)
add_library(${MCA_LIB_NAME} ${MCA_LIB_TYPE} ${MCA_LIB_SRCS})
set_target_properties(${MCA_LIB_NAME} PROPERTIES
        VERSION ${mca_VERSION}
        SOVERSION ${mca_VERSION}
        DEBUG_POSTFIX "-${mca_VERSION}d"
        RELEASE_POSTFIX "-${mca_VERSION}"
)
target_compile_features(${MCA_LIB_NAME} ${MCA_LIB_DEP_SCOPE} cxx_std_20)
target_include_directories(${MCA_LIB_NAME} ${MCA_LIB_DEP_SCOPE}
        ${OpenCV_INCLUDE_DIR}
        "${tlct_SOURCE_DIR}/src/include"
        "${PROJECT_SOURCE_DIR}/src/include"
)
target_link_libraries(${MCA_LIB_NAME} ${MCA_LIB_DEP_SCOPE}
        ${OpenCV_LIBS}
        tlct::lib
)

if (NOT MCA_HEADER_ONLY AND MCA_BUILD_SHARED_LIBS)
    target_compile_definitions(${MCA_LIB_NAME} PRIVATE "_MCA_BUILD_LIBS")
endif ()
