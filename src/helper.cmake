function(lvc_app_copy_dynlib tg)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${tg} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libLVC_preprocess> $<TARGET_FILE_DIR:${tg}>)
    endif ()
endfunction()
