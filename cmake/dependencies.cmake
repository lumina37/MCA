include(FetchContent)

find_package(OpenCV REQUIRED core imgcodecs imgproc highgui)

FetchContent_Declare(
        tlct
        GIT_REPOSITORY "https://github.com/SIGS-TZ/TLCT.git"
        GIT_TAG v0.6.0
)
FetchContent_MakeAvailable(tlct)

if (MCA_BUILD_TESTS)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )

    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(GTEST_LINKED_AS_SHARED_LIBRARY 1 CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)
endif ()
