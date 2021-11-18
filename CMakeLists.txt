cmake_minimum_required(VERSION 3.21)
project(ProcessSynchronization C)
project(abc C++)
set(CMAKE_C_STANDARD 11)

add_executable(ProcessSynchronization
        q1/main.c q1/entities.h q1/entities.c q1/functions.c q1/functions.h q1/globals.h q1/globals.c)
add_executable(abc client.cpp server_prog.cpp)

