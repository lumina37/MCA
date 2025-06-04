add_library(mca-lib INTERFACE)

target_compile_features(mca-lib INTERFACE cxx_std_23)
target_include_directories(mca-lib INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${OpenCV_INCLUDE_DIRS}>
)
target_link_libraries(mca-lib INTERFACE
        ${OpenCV_LIBS}
        tlct::lib::static
)

if (MSVC)
    target_compile_options(mca-lib INTERFACE /arch:AVX2)
else ()
    target_compile_options(mca-lib INTERFACE -mavx2)
endif ()

add_library(mca::lib ALIAS mca-lib)

add_subdirectory(bin)
