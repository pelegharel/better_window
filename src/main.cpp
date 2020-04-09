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

#include <array>
#include <functional>
#include <memory>
#include <stdio.h>
#include <tuple>
#include <type_traits>

template <typename T, auto destroy_fn>
using c_unique_ptr =
    std::unique_ptr<T,
                    std::integral_constant<decltype(destroy_fn), destroy_fn>>;

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **) {

  c_unique_ptr<GLFWwindow, glfwDestroyWindow> window{[] {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
      exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL3 example", NULL,
                            NULL);
  }()};

  glfwMakeContextCurrent(window.get());
  glfwSwapInterval(1); // Enable vsync

  gladLoadGL((GLADloadfunc)glfwGetProcAddress);

  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsLight();



  bool show_demo_window = true;
  bool show_theme_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  while (!glfwWindowShouldClose(window.get())) {
    glfwPollEvents();

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
    {
      static float f = 0.0f;
      static int counter = 0;
      ImGui::Text("Hello, world!"); // Display some text (you can use a format
                                    // string too)
      ImGui::SliderFloat("float", &f, 0.0f,
                         1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3(
          "clear color",
          (float *)&clear_color); // Edit 3 floats representing a color

      ImGui::Text("Windows");
      ImGui::Checkbox(
          "Demo Window",
          &show_demo_window); // Edit bools storing our windows open/close state
      ImGui::Checkbox("Themes Window",
                      &show_theme_window); // Edit bools storing our windows
                                           // open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

 //     ImGui::Text("Font Samples");
 //     ImGui::PushFont(font_cousine);
 //     ImGui::Text("Font Render Test - Cousine: Bit Test.123");
 //     ImGui::Text("Font Render Test - Cousine: XXXXXXXXXXXX");
 //     ImGui::PopFont();

 //     ImGui::PushFont(font_karla);
 //     ImGui::Text("Font Render Test - Karla: Bit Test.123");
 //     ImGui::Text("Font Render Test - Karla: XXXXXXXXXXXX");
 //     ImGui::PopFont();

 //     ImGui::PushFont(font_lato);
 //     ImGui::Text("Font Render Test - Lato: Bit Test.123");
 //     ImGui::Text("Font Render Test - Lato: XXXXXXXXXXXX");
 //     ImGui::PopFont();

      if (ImGui::Button("Button")) // Buttons return true when clicked (NB: most
                                   // widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. In most cases you will use an explicit
    // Begin/End pair to name your windows.
    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // 3. Show theme window
    if (show_theme_window) {
      ImGui::Begin("Themes", &show_theme_window);
      {
        const std::array themes = {"Light", "Dark", "Default"};

        constexpr std::array<void (*)(ImGuiStyle *), 3> funcs{
            {ImGui::StyleColorsLight, ImGui::StyleColorsDark,
             ImGui::StyleColorsClassic}};

        static int theme_current = 0;
        static int old_theme_current = 0;
        ImGui::Combo("theme-combo", &theme_current, themes.data(),
                     themes.size());
        if (old_theme_current != theme_current) {
          old_theme_current = theme_current;
          funcs[theme_current](nullptr);
        }

      }

      ImGui::End();
    }

    // 4. Show the ImGui demo window. Most of the sample code is in
    // ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (show_demo_window) {
      ImGui::SetNextWindowPos(
          ImVec2(650, 20),
          ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to
                                   // call this because positions are saved in
                                   // .ini file anyway. Here we just want to
                                   // make the demo initial state a bit more
                                   // friendly!
      ImGui::ShowDemoWindow(&show_demo_window);
    }

    //ImGui::PopFont();
    // Rendering
    ImGui::Render();
    glfwMakeContextCurrent(window.get());

    const auto [display_w, display_h] = [&window] {
      int display_w;
      int display_h;
      glfwGetFramebufferSize(window.get(), &display_w, &display_h);
      return std::tuple{display_w, display_h};
    }();

    {
      const auto [x, y, z, w] = clear_color;
      glViewport(0, 0, display_w, display_h);
      glClearColor(x, y, z, w);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window.get());
    glfwSwapBuffers(window.get());
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();

  return 0;
}
