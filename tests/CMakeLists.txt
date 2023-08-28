find_package(Catch2 3 REQUIRED)

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

add_executable(tests "test_config.cpp")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
lvc_linkage(tests)

include(CTest)
include(Catch)
catch_discover_tests(tests)
