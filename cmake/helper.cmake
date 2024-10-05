function(copy_dlls_if_needed name)
    if (WIN32)
        add_custom_command(TARGET ${name} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
                copy_if_different $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE:${name}> $<TARGET_FILE_DIR:${name}> COMMAND_EXPAND_LISTS)
    endif ()
endfunction()

find_package(Git QUIET)
if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE MCA_GIT_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()
if (NOT MCA_GIT_TAG)
    set(MCA_GIT_TAG "unknown")
endif ()

set(MCA_COMPILE_INFO "<tag:${MCA_GIT_TAG}> by [${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_SYSTEM_PROCESSOR})]")

set(MCA_CONFIGURE_DIR "${PROJECT_SOURCE_DIR}/src/include/mca/common")
configure_file("${MCA_CONFIGURE_DIR}/cmake.h.in" "${MCA_CONFIGURE_DIR}/cmake.h" @ONLY)
