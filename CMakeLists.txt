cmake_minimum_required(VERSION 3.23)
project(jump C)

set(CMAKE_C_STANDARD 99)

add_executable(jump main.c)
target_link_libraries(jump mingw32)
target_link_libraries(jump SDL2main)
target_link_libraries(jump SDL2)
target_link_libraries(jump SDL2_image)
target_link_libraries(jump SDL2_ttf)
