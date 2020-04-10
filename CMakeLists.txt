
cmake_minimum_required(VERSION 3.11)
project (better_window)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(modules)


list(APPEND MAIN_APP_SOURCES
       src/main.cpp
       src/imgui_impl/imgui_impl_opengl3.h
       src/imgui_impl/imgui_impl_opengl3.cpp
       src/imgui_impl/imgui_impl_glfw.h
       src/imgui_impl/imgui_impl_glfw.cpp
       src/glad/glad.h
       src/glad/glad.c
       src/KHR/khrplatform.h
        )

set(PROJECT_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

set(MAIN_APP_LIBRARIES imgui glfw)

list(APPEND MAIN_APP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(NAME better_window)
add_executable(${NAME} ${MAIN_APP_SOURCES})
set_source_files_properties(${PROJECT_CPP} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic")

target_include_directories(${NAME} PUBLIC ${MAIN_APP_INCLUDE_DIRS})
target_link_libraries(${NAME} imgui glfw)

