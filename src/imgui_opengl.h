#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>


#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#include <tuple>

namespace btw {

struct ImguiContext_glfw_opengl {
  GLFWwindow *window = nullptr;

  ImguiContext_glfw_opengl(int width, int height, const char *win_name);

  ImguiContext_glfw_opengl(const ImguiContext_glfw_opengl &) = delete;
  ImguiContext_glfw_opengl(ImguiContext_glfw_opengl &&) = delete;
  ImguiContext_glfw_opengl &
  operator=(const ImguiContext_glfw_opengl &) = delete;

  ImguiContext_glfw_opengl &operator=(ImguiContext_glfw_opengl &&) = delete;

  void render(ImVec4 clear_color);

  bool is_window_open() const;

  void start_frame();
  ~ImguiContext_glfw_opengl();
};
} // namespace btw
