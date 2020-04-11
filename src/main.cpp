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
#include "opencv2/videoio.hpp"

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

struct GLTexture {
  GLuint id = 0;
  explicit GLTexture(const cv::Mat &image) {

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, image.ptr());
  }

  GLTexture(const GLTexture &) = delete;
  GLTexture(GLTexture &&) = delete;
  GLTexture &operator=(const GLTexture &) = delete;
  GLTexture &operator=(GLTexture &&) = delete;

  ~GLTexture() {
    if (id) {
      glDeleteTextures(1, &id);
    }
  }
};

void main_loop(btw::ImguiContext_glfw_opengl &context) {

  cv::VideoCapture cap;
  cap.open(
      R"(/media/peleg/AAC8C7F7C8C7BFB5/downloads/Better.Call.Saul.S05E06.WEBRip.x264-ION10.mp4)");
  const auto frame_count = cap.get(cv::CAP_PROP_FRAME_COUNT);
  cv::Mat frame;

  ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

  std::vector<
      std::vector<std::tuple<cv::Rect, cv::Mat, std::unique_ptr<GLTexture>>>>
      im_rects_s(frame_count);

  int frame_old = 0;
  while (context.is_window_open()) {
    context.start_frame();
    ImGui::ShowMetricsWindow();

    ImGui::Begin("image", nullptr, ImGuiWindowFlags_NoSavedSettings);
    int frame_i;
    ImGui::SliderInt("slider", &frame_i, 0, frame_count - 1);
    auto &im_rects = im_rects_s[frame_i];

    ImGui::SameLine();
    if (ImGui::ArrowButton("<", ImGuiDir_Left)) {
      if (const auto non_empty = std::find_if(
              rbegin(im_rects_s) + (size(im_rects_s) - frame_i),
              rend(im_rects_s), [](const auto &v) { return !v.empty(); });
          non_empty != rend(im_rects_s)) {
        frame_i =
            size(im_rects_s) - std::distance(rbegin(im_rects_s), non_empty) - 1;
      }
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton(">", ImGuiDir_Right)) {
      if (const auto non_empty =
              std::find_if(next(begin(im_rects_s) + frame_i), end(im_rects_s),
                           [](const auto &v) { return !v.empty(); });
          non_empty != end(im_rects_s)) {
        frame_i = std::distance(begin(im_rects_s), non_empty);
      }
    }

    if (frame_i != frame_old) {
      cap.set(cv::CAP_PROP_POS_FRAMES, frame_i);
      cap.read(frame);
      frame_old = frame_i;
    }
    auto m = frame;
    const GLTexture gl_m(frame);

    ImGui::Text("%d", gl_m.id);

    ImGui::Image((void *)(intptr_t)gl_m.id, ImVec2(m.cols, m.rows));

    const auto [a, b] = ImGui::GetItemRectMin();

    auto *draw_list = ImGui::GetWindowDrawList();

    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemClicked()) {
      if (auto upmost_clicked = std::find_if(
              rbegin(im_rects), rend(im_rects),
              [pos = ImGui::GetMousePos(), a, b](const auto &datum) {
                const auto &[rect, _1, _2] = datum;
                const auto [x, y] = pos;
                return rect.contains({x - a, y - b});
              });
          upmost_clicked != rend(im_rects)) {
        im_rects.erase(std::next(upmost_clicked).base());
      }
    }
    if (ImGui::IsItemHovered()) {
      const auto [x0, y0] = ImGui::GetMousePos();
      const auto [dx, dy] = ImGui::GetMouseDragDelta();

      draw_list->AddRectFilled({x0 - dx, y0 - dy}, {x0, y0},
                               ImGui::GetColorU32({1, 1, 1, 0.5}));

      const cv::Rect im_rect(cv::Point(x0 - dx - a, y0 - dy - b),
                             cv::Point(x0 - a, y0 - b));

      const bool is_in_m =
          (im_rect & cv::Rect(0, 0, m.cols, m.rows)) == im_rect;

      if (ImGui::IsMouseReleased(0) && std::abs(dx) > 10 && is_in_m) {
        const cv::Mat sub_img = m(im_rect);

        im_rects.push_back(
            {im_rect, sub_img, std::make_unique<GLTexture>(sub_img)});
      }
    }

    for (const auto &[rect, _1, _2] : im_rects) {
      const auto [x_0, y_0] = rect.tl();
      const auto [x_1, y_1] = rect.br();
      draw_list->AddRectFilled({a + x_0, b + y_0}, {a + x_1, b + y_1},
                               ImGui::GetColorU32({0, 1, 0, 0.5}));
    }

    ImGui::End();
    {
      ImGui::Begin("Selected");
      for (const auto &[rect, sub, gl_mat] : im_rects) {

        ImGui::Value("v", gl_mat->id);
        ImGui::Image((void *)(intptr_t)gl_mat->id, ImVec2(sub.cols, sub.rows));
        [rect] {
          const auto [a, b] = rect.tl();
          const auto [c, d] = rect.br();
          ImGui::Text("rect %d, %d, %d, %d", a, b, c, d);
        }();
      }
      ImGui::End();
    }

    context.render({0, 0, 0, 0});
  }
}

int main(int, char **) {

  btw::ImguiContext_glfw_opengl context(1280, 720, "Better window");

  main_loop(context);

  return 0;
}
