cmake_minimum_required(VERSION 3.31)

set(THIS Hive)

project(${THIS} CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG        release-1.11.0
# )

# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(${THIS} STATIC ${LIB_DIR}/ThreadPool.cpp)
target_include_directories(${THIS} PUBLIC ${LIB_DIR})
target_link_libraries(${THIS} PUBLIC pthread)

add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC ${THIS})

# add_executable(ThreadPoolTests tests/ThreadPoolTests.cpp)
# target_link_libraries(ThreadPoolTests PRIVATE ${THIS} GTest::gtest_main)

# include(GoogleTest)
# gtest_discover_tests(ThreadPoolTests)

