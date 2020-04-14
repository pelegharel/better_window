// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

#include "imgui_opengl.h"

#include "opencv2/core/core.hpp"
#include "opencv2/dnn/dnn.hpp"
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
  int width;
  int height;

  explicit GLTexture(const cv::Mat &image)
      : width(image.cols), height(image.rows) {

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());

    constexpr std::array params{std::tuple{GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                                std::tuple{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
                                std::tuple{GL_TEXTURE_WRAP_S, GL_CLAMP},
                                std::tuple{GL_TEXTURE_WRAP_T, GL_CLAMP}};

    for (const auto &[p_name, p_value] : params) {
      glTexParameteri(GL_TEXTURE_2D, p_name, p_value);
    }

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

namespace ImGui {
void Image(const GLTexture &texture) {

  ImGui::Image((void *)(intptr_t)texture.id,
               ImVec2(texture.width, texture.height));
}
} // namespace ImGui

[[nodiscard]] auto face_detect(const cv::Mat &frame,
                               const GLTexture &frame_texture, cv::dnn::Net &n)
    -> std::vector<std::unique_ptr<GLTexture>> {

  const auto detections = [&n, &frame] {
    cv::Size s(300, 300);

    cv::Mat resized;
    cv::resize(frame, resized, s);
    const auto blob =
        cv::dnn::blobFromImage(resized, 1.0, s, cv::Scalar(104, 177, 123));

    n.setInput(blob);
    const cv::Mat detected = n.forward();
    return detected(std::vector{cv::Range(0, 1), cv::Range(0, 1),
                                cv::Range::all(), cv::Range::all()})
        .reshape(0, std::vector{detected.size[2], detected.size[3]});
  }();

  static float conf_thresh = 0.5;
  ImGui::SliderFloat("Conf Thresh", &conf_thresh, 0, 1);

  const auto dt = [&detections, conf_thresh] {
    std::vector<std::array<float, 4>> dt;

    for (int r = 0; r < detections.rows; ++r) {
      const auto row = detections.row(r);
      const auto conf = row.at<float>(2);
      if (conf > conf_thresh) {
        std::array<float, 4> rect;
        std::copy_n(row.ptr<float>(0, 3), 4, begin(rect));
        dt.push_back(rect);
      }
    }
    return dt;
  }();

  ImGui::Text("toal dec %ld", size(dt));

  auto *const draw_list = ImGui::GetWindowDrawList();

  ImGui::Image(frame_texture);
  const auto [a0, b0] = ImGui::GetItemRectMin();

  for (const auto [a, b, c, d] : dt) {
    draw_list->AddRectFilled({a0 + frame.cols * a, b0 + frame.rows * b},
                             {a0 + frame.cols * c, b0 + frame.rows * d},
                             ImGui::GetColorU32({0, 0, 1, 0.2}));
  }

  ImGui::Begin("Faces");

  std::vector<std::unique_ptr<GLTexture>> res;

  for (const auto [a, b, c, d] : dt) {
    const cv::Rect roi(cv::Point(frame.cols * a, frame.rows * b),
                       cv::Point(frame.cols * c, frame.rows * d));

    if ((roi & cv::Rect(0, 0, frame.cols, frame.rows)) == roi) {
      const cv::Mat face = frame(roi);
      res.emplace_back(std::make_unique<GLTexture>(face));
      ImGui::Image(*res.back());
    }
  }

  ImGui::End();

  return res;
}
void main_loop(btw::ImguiContext_glfw_opengl &context, cv::dnn::Net &n) {

  cv::VideoCapture cap;
  cap.open(
      R"(/media/peleg/AAC8C7F7C8C7BFB5/downloads/Better.Call.Saul.S05E06.WEBRip.x264-ION10.mp4)");
  const auto frame_count = cap.get(cv::CAP_PROP_FRAME_COUNT);

  cv::Mat frame;
  if (!cap.read(frame)) {
    return;
  }

  ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

  std::vector<
      std::vector<std::tuple<cv::Rect, cv::Mat, std::unique_ptr<GLTexture>>>>
      im_rects_s(frame_count);

  int frame_old = 0;
  int frame_i = 0;
  while (context.is_window_open()) {
    context.start_frame();
    ImGui::ShowMetricsWindow();

    ImGui::Begin("image", nullptr, ImGuiWindowFlags_NoSavedSettings);
    ImGui::SliderInt("slider", &frame_i, 0, frame_count - 1);
    auto &im_rects = im_rects_s[frame_i];

    if (frame_i != frame_old) {
      cap.set(cv::CAP_PROP_POS_FRAMES, frame_i);
      cap.read(frame);
      frame_old = frame_i;
    }
    auto m = frame;
    const GLTexture gl_m(frame);

    const auto face_textures = face_detect(frame, gl_m, n);

    ImGui::End();

    context.render({0, 0, 0, 0});
  }
}

int main(int, char **) {
  cv::dnn::Net n = cv::dnn::readNetFromCaffe(
      R"(/media/peleg/AAC8C7F7C8C7BFB5/deep_learning_tut/deep-learning-face-detection/deploy.prototxt.txt)",
      R"(/media/peleg/AAC8C7F7C8C7BFB5/deep_learning_tut/deep-learning-face-detection/res10_300x300_ssd_iter_140000.caffemodel)");

  btw::ImguiContext_glfw_opengl context(1280, 720, "Better window");

  main_loop(context, n);

  return 0;
}
