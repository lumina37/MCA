function(copy_dlls_if_needed name)
    if (MCA_BUILD_SHARED_LIBS AND WIN32)
        add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                copy_if_different $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE_DIR:${name}> COMMAND_EXPAND_LISTS)
    endif ()
endfunction()

set(MCA_CONFIGURE_DIR "${PROJECT_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIGURE_DIR}/cmake.h.in" "${MCA_CONFIGURE_DIR}/cmake.h")
