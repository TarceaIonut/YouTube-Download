cmake_minimum_required(VERSION 3.29)
project(Youtube_Download_v2 C)

set(CMAKE_C_STANDARD 23)

add_executable(Youtube_Download_v2 main.c
        env.c
        directory_search.c
        directory_rename.c
        directory.c)
