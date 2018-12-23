# cmake_minimum_required(VERSION <specify CMake version here>)
project(brainfork-interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(brainfork-interpreter main.cpp brainfork_executor.cpp brainfork_executor.h)
