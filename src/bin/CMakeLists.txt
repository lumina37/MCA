function(mca_add_executable name srcs)
    add_executable(${name} ${srcs})
    target_link_libraries(${name} PRIVATE mca argparse)

    if (TLCT_ENABLE_LTO)
        set_target_properties(${name} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION ON
        )
    endif ()

    copy_dlls_if_needed(${name})
endfunction()

mca_add_executable(mca-preproc "main_preproc.cpp")
mca_add_executable(mca-postproc "main_postproc.cpp")
