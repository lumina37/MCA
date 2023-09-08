function(lvc_app_linkage app lib)
    target_link_libraries(${app} PUBLIC lib)
    target_compile_features(${app} PRIVATE cxx_std_20)

    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${app} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${app}>)
    endif ()
endfunction()
