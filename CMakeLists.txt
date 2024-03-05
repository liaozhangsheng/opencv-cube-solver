cmake_minimum_required(VERSION 3.20)
project(opencv-cube-solver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -g)

set(CMAKE_BUILD_TYPE release)

add_subdirectory(src)

add_subdirectory(cppkociemba)
