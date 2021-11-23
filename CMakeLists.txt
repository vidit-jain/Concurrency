cmake_minimum_required(VERSION 3.21)
project(ProcessSynchronization C)
set(CMAKE_C_STANDARD 11)

add_executable(ProcessSynchronization
        q1/main.c q1/entities.h q1/entities.c q1/functions.c q1/functions.h q1/globals.h q1/globals.c q2/entities.c q2/entities.h q2/functions.c q2/functions.h q2/globals.c q2/globals.h q2/main.c)

