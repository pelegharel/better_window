
list(APPEND MAIN_APP_SOURCES
        main.cpp
        imgui_impl/imgui_impl_opengl3.h
        imgui_impl/imgui_impl_opengl3.cpp
        imgui_impl/imgui_impl_glfw.h
        imgui_impl/imgui_impl_glfw.cpp
        glad/glad.h
        glad/glad.c
        KHR/khrplatform.h
        )

set(MAIN_APP_LIBRARIES imgui glfw)

list(APPEND MAIN_APP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(NAME better_window)
add_executable(${NAME} ${MAIN_APP_SOURCES})
target_include_directories(${NAME} PUBLIC ${MAIN_APP_INCLUDE_DIRS})
target_link_libraries(${NAME} imgui glfw)

