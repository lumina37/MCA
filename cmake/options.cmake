include(CMakeDependentOption)

option(MCA_HEADER_ONLY "Enable the header-only mode" OFF)
cmake_dependent_option(MCA_BUILD_SHARED_LIBS "Specifies the type of MCA to build" ON
        "NOT MCA_HEADER_ONLY AND BUILD_SHARED_LIBS" OFF)
cmake_dependent_option(MCA_ENABLE_LTO "Enable full link-time-optimizations (LTO)" OFF
        "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

if (MSVC)
    option(MCA_WITH_STATIC_CRT "Link with STATIC CRT" OFF)
    set(MSVC_RUNTIME_TYPE MultiThreaded$<$<NOT:$<BOOL:${MCA_WITH_STATIC_CRT}>>:DLL>)
endif ()
