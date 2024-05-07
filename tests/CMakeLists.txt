function(mca_add_test name srcs)
    add_executable(${name} ${srcs})
    target_compile_features(${name} PRIVATE cxx_std_20)
    target_link_libraries(${name} PRIVATE mca GTest::gtest_main)
    copy_dlls_if_needed(${name})
    gtest_discover_tests(${name})
endfunction()

mca_add_test(test_config "test_config.cpp")
