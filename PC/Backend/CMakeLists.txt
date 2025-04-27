cmake_minimum_required(VERSION 3.5)
project(sensordump C)

set(CMAKE_C_STANDARD 99)

add_executable(sensordump main.c)

# tell CMake to link against "-lsensors"
target_link_libraries(sensordump PRIVATE sensors)
