cmake_minimum_required(VERSION 3.0.0)
project(FileLock VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(FileLock FileLock.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_executable(
    FileLockTest

    FileLock.cpp
    FileLockTest.cpp
)

target_link_libraries(
    FileLockTest
    gtest
    glog
    gflags
    # gtest_main
    pthread
)
