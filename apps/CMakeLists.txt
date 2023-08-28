add_executable(draw_circles "draw_circles.cpp")
add_executable(draw_used_area "draw_used_area.cpp")
add_executable(backward "main_backward.cpp")
add_executable(forward "main_forward.cpp")
add_executable(process "process.cpp")

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

foreach (tg draw_circles draw_used_area backward forward process)
    lvc_linkage(${tg})
endforeach ()
