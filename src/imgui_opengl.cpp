#include "imgui_opengl.h"
#include <iostream>

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "Glfw Error " << error << ':' << description << '\n';
}

btw::ImguiContext_glfw_opengl::ImguiContext_glfw_opengl(int width, int height,
                                                        const char *win_name) {
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

void btw::ImguiContext_glfw_opengl::render(ImVec4 clear_color) {
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

bool btw::ImguiContext_glfw_opengl::is_window_open() const {
  return !glfwWindowShouldClose(window);
}

void btw::ImguiContext_glfw_opengl::start_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glfwPollEvents();
}

btw::ImguiContext_glfw_opengl::~ImguiContext_glfw_opengl() {
  glfwDestroyWindow(window);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}
