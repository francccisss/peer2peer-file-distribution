cmake_minimum_required(VERSION 4.0)
project(distributed_file_system C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(distributed_file_system
        dynamic_array.c
        dynamic_array.h
        main.c
        nodes.c)
