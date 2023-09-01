add_executable(dbg_draw_circles "draw_circles.cpp")
add_executable(dbg_draw_used_area "draw_used_area.cpp")
add_executable(dbg_backward "main_backward.cpp")
add_executable(dbg_forward "main_forward.cpp")
add_executable(main_process "process.cpp")

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

foreach (tg dbg_draw_circles dbg_draw_used_area dbg_backward dbg_forward main_process)
    target_compile_features(${tg} PRIVATE cxx_std_20)
    lvc_linkage(${tg})
endforeach ()
