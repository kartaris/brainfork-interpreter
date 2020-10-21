cmake_minimum_required(VERSION 3.17)

project(tests)

include_directories(include)
include_directories(../)

set(test_source ../brainfork_executor.cpp test.cpp)
add_executable(tests ${test_source})

add_test(NAME catchTests COMMAND tests)
