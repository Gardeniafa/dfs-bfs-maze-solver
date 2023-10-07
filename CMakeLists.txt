cmake_minimum_required(VERSION 3.26)
project(maze23 C)

set(CMAKE_C_STANDARD 11)
add_definitions(-DON_CLION_CONSOLE)
add_executable(maze23 main.c)
