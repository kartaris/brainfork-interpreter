# cmake_minimum_required(VERSION <specify CMake version here>)
project(brainfork-interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(brainfork-interpreter main.cpp brainfork_parser.cpp brainfork_parser.h)