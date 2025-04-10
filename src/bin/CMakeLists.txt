function(mca_add_executable name srcs)
    add_executable(${name} ${srcs})
    target_link_libraries(${name} PRIVATE
            mca::lib
            argparse::argparse
    )

    if (MCA_ENABLE_LTO)
        set_target_properties(${name} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION ON
        )
    endif ()

    if (MSVC)
        set_target_properties(${name} PROPERTIES
                MSVC_RUNTIME_LIBRARY ${_MCA_MSVC_RUNTIME_LIBRARY}
        )
        target_compile_definitions(${name} PRIVATE "_CRT_SECURE_NO_WARNINGS")
    endif ()

    tlct_copy_dlls_if_needed(${name})
endfunction()

mca_add_executable(mca-bin "main.cpp")
set_target_properties(mca-bin PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
)
