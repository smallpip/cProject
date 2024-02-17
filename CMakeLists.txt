cmake_minimum_required(VERSION 3.0)
project(HELLO)
add_executable(main main.cpp)
target_compile_options(main PRIVATE -g -O2 )