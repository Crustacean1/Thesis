cmake_minimum_required(VERSION 3.15)

project(deep_space VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 23)
set(CXX_STANDARD_REQUIRED ON)
add_compile_options(-g)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(./deep_space/)
add_subdirectory(./demo)
