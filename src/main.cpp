// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

#include "imgui_opengl.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <tuple>
#include <type_traits>
#include <vector>

GLuint load_to_opengl(const cv::Mat &image) {

  GLuint texture;

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR,
               GL_UNSIGNED_BYTE, image.ptr());

  return texture;
}

void main_loop(btw::ImguiContext_glfw_opengl &context) {

  cv::Mat m = cv::imread("MyImage01.jpg", cv::IMREAD_COLOR);
  auto gl_m = load_to_opengl(m);

  std::vector<cv::Rect> screen_rects;

  while (context.is_window_open()) {
    context.start_frame();

    const auto [m_x, m_y] = ImGui::GetCursorStartPos();
    ImGui::Text("rect_min %f, %f", m_x, m_y);
    {
      ImGui::Begin("image", nullptr, ImGuiWindowFlags_NoMove);
      ImGui::Image((void *)(intptr_t)gl_m, ImVec2(m.cols, m.rows));

      auto *draw_list = ImGui::GetWindowDrawList();

      if (ImGui::IsItemHovered()) {
        const auto [x0, y0] = ImGui::GetMousePos();
        const auto [dx, dy] = ImGui::GetMouseDragDelta();

        draw_list->AddRectFilled({x0 - dx, y0 - dy}, {x0, y0},
                                 ImGui::GetColorU32({1, 1, 1, 0.5}));

        if (ImGui::IsMouseReleased(0)) {
          screen_rects.emplace_back(cv::Point(x0 - dx, y0 - dy),
                                    cv::Point(x0, y0));
        }
      }

      if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemClicked()) {
        auto upmost_clicked =
            std::find_if(rbegin(screen_rects), rend(screen_rects),
                         [pos = ImGui::GetMousePos()](const auto &rect) {
                           const auto [x, y] = pos;
                           return rect.contains({x, y});
                         });
        screen_rects.erase(std::next(upmost_clicked).base());
      }

      for (const auto &rect : screen_rects) {
        const auto [x_0, y_0] = rect.tl();
        const auto [x_1, y_1] = rect.br();
        draw_list->AddRectFilled({x_0, y_0}, {x_1, y_1},
                                 ImGui::GetColorU32({0, 1, 0, 0.5}));
      }

      ImGui::End();
    }

    ImGui::ShowDemoWindow();

    context.render({0, 0, 0, 0});
  }
}

int main(int, char **) {
  btw::ImguiContext_glfw_opengl context(1280, 720, "Better window");

  main_loop(context);

  return 0;
}
