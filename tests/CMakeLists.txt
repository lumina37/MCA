find_package(Catch2 3 REQUIRED)

include(${PROJECT_SOURCE_DIR}/src/helper.cmake)

add_executable(tests "test_config.cpp")
target_link_libraries(tests PRIVATE libLVC_preprocess Catch2::Catch2WithMain)
lvc_app_copy_dynlib(tests)

include(CTest)
include(Catch)
catch_discover_tests(tests)
