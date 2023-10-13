cmake_minimum_required(VERSION 3.14)
project(zcalc_test)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "")

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CLN REQUIRED cln>=1.3)

enable_testing()
add_subdirectory(../cas zcas)
add_executable(zcalc_test math.cpp)
target_include_directories(zcalc_test PUBLIC ../src ${CLN_INCLUDE_DIRS})
target_link_libraries(zcalc_test zcalc-cas GTest::gtest_main ${CLN_LIBRARIES})
target_compile_options(zcalc_test PUBLIC -g ${CLN_CFLAGS_OTHER})
include(GoogleTest)
gtest_discover_tests(zcalc_test)