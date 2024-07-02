include(FetchContent)

find_package(OpenCV REQUIRED core imgcodecs imgproc)

set(TLCT_HEADER_ONLY ON CACHE BOOL "")
FetchContent_Declare(
        tlct
        GIT_REPOSITORY "https://github.com/SIGS-TZ/TLCT.git"
        GIT_TAG v0.10.2
)
FetchContent_MakeAvailable(tlct)

FetchContent_Declare(
        argparse
        GIT_REPOSITORY "https://github.com/p-ranav/argparse.git"
        GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

if (MCA_BUILD_TESTS)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )

    set(BUILD_GMOCK OFF CACHE INTERNAL "" FORCE)
    set(GTEST_LINKED_AS_SHARED_LIBRARY 1 CACHE INTERNAL "" FORCE)
    set(gtest_force_shared_crt ON CACHE INTERNAL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)
endif ()
