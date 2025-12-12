cmake_minimum_required(VERSION 3.25)
project(SEK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
  BUILD_DIR="${CMAKE_CURRENT_SOURCE_DIR}/build"
)

include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    SYSTEM
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.0
    SYSTEM
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.3.1
    SYSTEM
)
FetchContent_MakeAvailable(cxxopts)

add_subdirectory(src)
add_subdirectory(tests)
