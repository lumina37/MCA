function(mca_add_library name srcs)
    if (BUILD_SHARED_LIBS)
        add_library(${name} SHARED ${srcs})
        target_compile_definitions(${name} PUBLIC "_LVC_EXPORTS")
    else ()
        add_library(${name} STATIC ${srcs})
    endif ()

    target_include_directories(${name} PUBLIC ${MCA_INCLUDE_DIR})
    target_link_libraries(${name} PUBLIC ${MCA_LINK_LIBS})
    target_compile_features(${name} PUBLIC cxx_std_20)
endfunction()

set(MCA_SRCS "process/preproc.cpp" "process/postproc.cpp" "process/debug.cpp" "config/config.cpp" "config/parser.cpp")

# Remember to add "" here, or the function will only get the first item.
mca_add_library(libMCA "${MCA_SRCS}")
