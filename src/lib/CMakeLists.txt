if (MCA_HEADER_ONLY)
    set(MCA_LIB_TYPE "INTERFACE")
    set(MCA_LIB_DEP_SCOPE "INTERFACE")
else ()
    if (MCA_BUILD_SHARED_LIBS)
        set(MCA_LIB_TYPE "SHARED")
        set(MCA_LIB_COMPILE_DEFS "_MCA_BUILD_LIBS")
    else ()
        set(MCA_LIB_TYPE "STATIC")
    endif ()
    set(MCA_LIB_DEP_SCOPE "PUBLIC")
    set(MCA_LIB_SRCS "lib.cpp")
endif ()

set(MCA_LIB_NAME mca)
add_library(${MCA_LIB_NAME} ${MCA_LIB_TYPE} ${MCA_LIB_SRCS})
set_target_properties(${MCA_LIB_NAME} PROPERTIES
        VERSION ${MCA_VERSION}
        SOVERSION ${MCA_VERSION}
        DEBUG_POSTFIX "-${MCA_VERSION}d"
        RELEASE_POSTFIX "-${MCA_VERSION}")
target_compile_definitions(${MCA_LIB_NAME} PRIVATE ${MCA_LIB_COMPILE_DEFS})
target_include_directories(${MCA_LIB_NAME} ${MCA_LIB_DEP_SCOPE}
        ${OpenCV_INCLUDE_DIR}
        "${tlct_SOURCE_DIR}/src/include"
        "${PROJECT_SOURCE_DIR}/src/include")
target_link_libraries(${MCA_LIB_NAME} ${MCA_LIB_DEP_SCOPE}
        ${OpenCV_LIBS}
        tlct)
