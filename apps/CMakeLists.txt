# add_executable(main "main_forward.cpp")
add_executable(main "process.cpp")

target_link_libraries(main PUBLIC libLVC_preprocess)

target_compile_features(main PUBLIC cxx_std_11)

if (BUILD_SHARED_LIBS)
    add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libLVC_preprocess> $<TARGET_FILE_DIR:main>)
endif ()
