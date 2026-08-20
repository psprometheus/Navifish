cmake_minimum_required(VERSION 3.10)
project(SigmaChess CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp)

target_compile_options(main PRIVATE -mbmi2 -mavx2 -mpopcnt)
