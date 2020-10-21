cmake_minimum_required(VERSION 3.17)

project(brainfork-interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(brainfork-interpreter main.cpp brainfork_executor.cpp brainfork_executor.h)

enable_testing()
add_subdirectory(test tests)
