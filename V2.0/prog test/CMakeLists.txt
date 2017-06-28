cmake_minimum_required(VERSION 3.6)
project(test_com)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl")

set(SOURCE_FILES main.c)
add_executable(test_com ${SOURCE_FILES})
target_link_libraries(test_com curl)