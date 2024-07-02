include(FetchContent)

find_package(OpenCV REQUIRED core imgcodecs imgproc)

set(MCA_TLCT_PATH "https://github.com/SIGS-TZ/TLCT/archive/refs/tags/v0.14.0.tar.gz" CACHE STRING
        "Specifies the path of TLCT")

set(TLCT_HEADER_ONLY ${MCA_HEADER_ONLY} CACHE BOOL "")
set(TLCT_BUILD_SHARED_LIBS ${MCA_BUILD_SHARED_LIBS} CACHE BOOL "")
FetchContent_Declare(
        tlct
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL ${MCA_TLCT_PATH}
)
FetchContent_MakeAvailable(tlct)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(MCA_ARGPARSE_PATH "https://github.com/p-ranav/argparse/archive/refs/tags/v3.1.tar.gz" CACHE STRING
            "Specifies the path of argparse")
    set(TLCT_ARGPARSE_PATH ${MCA_ARGPARSE_PATH} CACHE STRING "")
    FetchContent_Declare(
            argparse
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            URL ${MCA_ARGPARSE_PATH}
    )
    FetchContent_MakeAvailable(argparse)
endif ()
