cmake_minimum_required(VERSION 3.5)
project(Backend C)

set(CMAKE_C_STANDARD 99)

add_executable(Backend main.c)

target_link_libraries(Backend PRIVATE sensors)
