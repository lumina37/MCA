include(CMakeDependentOption)

option(MCA_BUILD_TESTS "Build tests" OFF)
option(MCA_HEADER_ONLY "Enable the header-only mode" OFF)
cmake_dependent_option(MCA_BUILD_SHARED_LIBS "Specifies the type of MCA to build" ON
        "NOT MCA_HEADER_ONLY" OFF)

if (MSVC)
    option(MCA_WITH_STATIC_CRT "Link with STATIC CRT (this wont work if OpenCV is already linked with SHARED CRT)" OFF)
    set(MSVC_RUNTIME_TYPE MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${MCA_WITH_STATIC_CRT}>>:DLL>)
endif ()
