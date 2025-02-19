cmake_minimum_required(VERSION 3.28)
project(codecrafters_shell_c C)

set(CMAKE_C_STANDARD 99)

add_executable(codecrafters_shell app/main.c
        app/builtins.h
        app/builtins.c
        app/utils.h
        app/utils.c
        app/shell.c
        app/shell.h
        app/structs.c
        app/structs.h
)
