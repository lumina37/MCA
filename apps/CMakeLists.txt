add_executable(dbg_draw_circles "draw_circles.cpp")
add_executable(dbg_draw_used_area "draw_used_area.cpp")
add_executable(dbg_backward "main_backward.cpp")
add_executable(dbg_forward "main_forward.cpp")
add_executable(main_preproc "main_preproc.cpp")
add_executable(main_postproc "main_postproc.cpp")

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

foreach (app dbg_draw_circles dbg_draw_used_area dbg_backward dbg_forward main_preproc main_postproc)
    lvc_app_linkage(${app} libMCA)
endforeach ()
