add_executable(main "process.cpp")

find_package(Boost REQUIRED)

target_include_directories(main PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(main PUBLIC libLVC_preprocess ${Boost_LIBRARIES})

if (BUILD_SHARED_LIBS)
    add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libLVC_preprocess> $<TARGET_FILE_DIR:main>)
endif ()
