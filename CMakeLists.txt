
cmake_minimum_required(VERSION 3.11)

project(better_window LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions("-DCPP_SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/\"")


add_subdirectory(modules)
add_subdirectory(src)

