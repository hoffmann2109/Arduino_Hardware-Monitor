cmake_minimum_required(VERSION 3.21)
project(PC C)

set(CMAKE_C_STANDARD 11)

find_library(SENSORS_LIB sensors)
include_directories(/usr/include)

add_executable(PC main.c)
target_link_libraries(PC sensors)
