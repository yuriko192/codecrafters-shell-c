cmake_minimum_required(VERSION 3.28)
project(codecrafters_shell_c C)

set(CMAKE_C_STANDARD 23)

add_executable(codecrafters_shell app/main.c
        app/builtins.h
        app/builtins.c
        app/utils.h
        app/utils.c
)
