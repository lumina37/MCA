add_executable(dbg_draw_circles "draw_circles.cpp")
add_executable(dbg_draw_used_area "draw_used_area.cpp")
add_executable(dbg_backward "main_backward.cpp")
add_executable(dbg_forward "main_forward.cpp")
add_executable(main_process_modern "process.cpp")
add_executable(main_process_legacy "process.cpp")

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

foreach (tg dbg_draw_circles dbg_draw_used_area dbg_backward dbg_forward main_process_modern)
    target_link_libraries(${tg} PUBLIC libLVC_preprocess)
    target_compile_features(${tg} PRIVATE cxx_std_20)
    lvc_app_copy_dynlib(${tg})
endforeach ()

target_link_libraries(main_process_legacy PUBLIC libLVC_preprocess_legacy)
target_compile_features(main_process_legacy PRIVATE cxx_std_20)
lvc_app_copy_dynlib(main_process_legacy)
