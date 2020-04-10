// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <tuple>
#include <type_traits>
#include <cmath>

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "Glfw Error " << error << ':' << description << '\n';
}

struct ImguiContext_glfw_opengl {
  GLFWwindow *window = nullptr;

  ImguiContext_glfw_opengl(int width, int height, const char *win_name) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
      exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, win_name, nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGL((GLADloadfunc)glfwGetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
  }

  ImguiContext_glfw_opengl(const ImguiContext_glfw_opengl &) = delete;
  ImguiContext_glfw_opengl(ImguiContext_glfw_opengl &&) = delete;
  ImguiContext_glfw_opengl &
  operator=(const ImguiContext_glfw_opengl &) = delete;

  ImguiContext_glfw_opengl &operator=(ImguiContext_glfw_opengl &&) = delete;

  void render(ImVec4 clear_color) {
    ImGui::Render();
    glfwMakeContextCurrent(window);

    const auto [display_w, display_h] = [&] {
      int display_w;
      int display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      return std::tuple{display_w, display_h};
    }();

    const auto [x, y, z, w] = clear_color;
    glViewport(0, 0, display_w, display_h);
    glClearColor(x, y, z, w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
  }

  bool is_window_open() const { return !glfwWindowShouldClose(window); }

  void start_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    glfwPollEvents();
  }
  ~ImguiContext_glfw_opengl() {
    glfwDestroyWindow(window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
  }
};

void main_loop(ImguiContext_glfw_opengl &context) {

  while (context.is_window_open()) {
    context.start_frame();

    {
      ImGui::PlotLines(
          "Sin", [](void *data, int idx) { return sinf(idx * 0.2F); }, nullptr,
          100);
    }
    ImGui::ShowDemoWindow();

    context.render({0, 0, 0, 0});
  }
}

int main(int, char **) {

  ImguiContext_glfw_opengl context(1280, 720, "Better window");

  main_loop(context);

  return 0;
}
