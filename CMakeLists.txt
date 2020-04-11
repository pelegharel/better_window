
cmake_minimum_required(VERSION 3.11)
project (better_window)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(modules)


aux_source_directory(src/imgui_impl MAIN_APP_SOURCES)
aux_source_directory(src/glad MAIN_APP_SOURCES)
aux_source_directory(src/KHR MAIN_APP_SOURCES)

set(PROJECT_CPP src/main.cpp src/imgui_opengl.cpp)

set(MAIN_APP_LIBRARIES imgui glfw)

list(APPEND MAIN_APP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(NAME better_window)
add_executable(${NAME} ${MAIN_APP_SOURCES} ${PROJECT_CPP})
set_source_files_properties(${PROJECT_CPP} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -pedantic")

target_include_directories(${NAME} PUBLIC ${MAIN_APP_INCLUDE_DIRS})
target_link_libraries(${NAME} imgui glfw ${OpenCV_LIBS})

