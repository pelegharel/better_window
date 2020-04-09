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
#include <memory>
#include <stdio.h>
#include <tuple>
#include <type_traits>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

struct ImguiContext_glfw_opengl {
  GLFWwindow *window = nullptr;

  ImguiContext_glfw_opengl() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
      exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL3 example", nullptr,
                              nullptr);

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

  ~ImguiContext_glfw_opengl() {
    glfwDestroyWindow(window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
  }
};

void start_imgui_frame() {
  // Start the ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void main_loop(ImguiContext_glfw_opengl &context) {
  struct State {
    bool show_demo_window = true;
    bool show_theme_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    float f = 0.0f;
    int counter = 0;

    int theme_current = 0;
  };

  State s;

  while (context.is_window_open()) {
    s = [&context, s] {
      State ns = s;

      glfwPollEvents();
      start_imgui_frame();

      // 1. Show a simple window.
      // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
      // automatically appears in a window called "Debug".
      [&ns, s] {
        ImGui::Text("Hello, world!"); // Display some text (you can use a format
                                      // string too)
        ImGui::SliderFloat(
            "float", &ns.f, 0.0f,
            1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color",
            (float *)&ns.clear_color); // Edit 3 floats representing a color

        ImGui::Text("Windows");
        ImGui::Checkbox("Demo Window",
                        &ns.show_demo_window); // Edit bools storing our windows
                                               // open/close state
        ImGui::Checkbox("Themes Window",
                        &ns.show_theme_window); // Edit bools storing our
                                                // windows open/close state
        ImGui::Checkbox("Another Window", &ns.show_another_window);

        if (ImGui::Button("Button")) {
          ns.counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", s.counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
      }();

      // 2. Show another simple window. In most cases you will use an explicit
      // Begin/End pair to name your windows.
      if (s.show_another_window) {
        ImGui::Begin("Another Window", &ns.show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
          ns.show_another_window = false;
        }
        ImGui::End();
      }

      // 3. Show theme window
      if (s.show_theme_window) {
        ImGui::Begin("Themes", &ns.show_theme_window);
        {
          const std::array themes = {"Light", "Dark", "Default"};

          constexpr std::array<void (*)(ImGuiStyle *), size(themes)> funcs{
              {ImGui::StyleColorsLight, ImGui::StyleColorsDark,
               ImGui::StyleColorsClassic}};

          ImGui::Combo("theme-combo", &ns.theme_current, themes.data(),
                       themes.size());

          funcs[s.theme_current](nullptr);
        }

        ImGui::End();
      }

      // 4. Show the ImGui demo window. Most of the sample code is in
      // ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
      if (s.show_demo_window) {
        ImGui::ShowDemoWindow(&ns.show_demo_window);
      }

      context.render(s.clear_color);

      return ns;
    }();
  }
}

int main(int, char **) {

  ImguiContext_glfw_opengl context;

  main_loop(context);

  return 0;
}
