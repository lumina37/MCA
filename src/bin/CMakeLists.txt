function(mca_add_executable name srcs)
    add_executable(${name} ${srcs})
    target_link_libraries(${name} PRIVATE mca-lib)

    if (TLCT_ENABLE_LTO)
        set_target_properties(${name} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION ON
        )
    endif ()

    copy_dlls_if_needed(${name})
endfunction()

mca_add_executable(mca-bin "main.cpp")
set_target_properties(mca-bin PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
)
