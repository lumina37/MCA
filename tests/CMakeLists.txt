function(mca_add_test name srcs)
    add_executable(${name} ${srcs})
    target_include_directories(${name} PRIVATE ${MCA_INCLUDE_DIR})
    target_link_libraries(${name} PRIVATE libMCA GTest::gtest_main)
    gtest_discover_tests(${name})
endfunction()

mca_add_test(test_config "test_config.cpp")
