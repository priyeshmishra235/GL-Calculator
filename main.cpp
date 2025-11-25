#include "fontLoader.hpp"
#include "glad/glad.h"
#include "renderer.hpp"
#include "shader.hpp"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

struct Btn {
  float x, y, w, h;
  std::string label;
};

bool hit(float mx, float my, float x, float y, float w, float h) {
  return mx >= x && mx <= x + w && my >= y && my <= y + h;
}

int main() {
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  std::string appln = "GL Calculator";
  GLFWwindow *win =
      glfwCreateWindow(400, 600, "GL Calculator", nullptr, nullptr);
  if (!win) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(win);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    return -1;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Renderer renderer;
  renderer.init();

  GLuint prog = Shader::createProgram();
  Font font = loadFont("digital-7.ttf", 22);
  Font fontAppln = loadFont("debrose.ttf", 22);

  std::string disp = "0.00000000";
  double left = 0;
  char op = 0;
  bool typing = false;
  bool lastMouseState = false;

  std::vector<Btn> btns = {{20, 40, 360, 80, ""},   {20, 140, 80, 60, "7"},
                           {120, 140, 80, 60, "8"}, {220, 140, 80, 60, "9"},
                           {300, 140, 80, 60, "/"}, {20, 220, 80, 60, "4"},
                           {120, 220, 80, 60, "5"}, {220, 220, 80, 60, "6"},
                           {300, 220, 80, 60, "*"}, {20, 300, 80, 60, "1"},
                           {120, 300, 80, 60, "2"}, {220, 300, 80, 60, "3"},
                           {300, 300, 80, 60, "-"}, {20, 380, 180, 60, "0"},
                           {220, 380, 77, 60, "."}, {303, 380, 80, 60, "+"},
                           {20, 460, 180, 60, "C"}, {220, 460, 160, 60, "="}};

  while (!glfwWindowShouldClose(win)) {
    glfwPollEvents();

    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(win, true);
    }
    int winW, winH;
    glfwGetFramebufferSize(win, &winW, &winH);
    glViewport(0, 0, winW, winH);

    renderer.setProjection(prog, 400, 600);

    double mx, my;
    glfwGetCursorPos(win, &mx, &my);

    bool currentMouseState = glfwGetMouseButton(win, 0) == GLFW_PRESS;
    bool clicked = currentMouseState && !lastMouseState;
    lastMouseState = currentMouseState;

    glClearColor(0.12, 0.12, 0.13, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float white[4] = {1, 1, 1, 1};
    for (int i{0}; i < btns.size(); ++i) {
      auto b = btns[i];

      float c[4] = {0.9, 0.5, 0.0, 1.0};
      float white[4] = {1, 1, 1, 1};

      if (i == 0) {
        c[0] = 0.5f;
        c[1] = 0.5f;
        c[2] = 0.5f;
        c[3] = 1.0f;
      }
      if (i == 1 || i == 2 || i == 3 || i == 5 || i == 6 || i == 7 || i == 9 ||
          i == 10 || i == 11 || i == 13) {
        c[0] = 0.9f;
        c[1] = 0.9f;
        c[2] = 0.9f;
        c[3] = 1.0f;
        white[0] = 0;
        white[1] = 0;
        white[2] = 0;
        white[3] = 1;
      }

      if (i != 0 && hit((float)mx, (float)my, b.x, b.y, b.w, b.h)) {
        c[0] = 0.5f;
        c[1] = 0.5f;
        c[2] = 0.00f;
      }

      renderer.drawRect(prog, b.x, b.y, b.w, b.h, c);

      float textW = 0;
      for (char ch : b.label)
        textW += font.glyphs[ch].xAdvance;
      float textX = b.x + (b.w - textW) / 2.0f;
      float textY = b.y + (b.h - font.size) / 2.0f + 5;

      renderer.drawText(font, prog, b.label, textX, textY, white);

      if (clicked && hit((float)mx, (float)my, b.x, b.y, b.w, b.h)) {
        if (b.label == "C") {
          disp = "0";
          left = 0;
          op = 0;
          typing = false;
        } else if (b.label == "=") {
          double r = 0;
          try {
            r = std::stod(disp);
          } catch (...) {
            r = 0;
          }

          if (op == '+')
            left += r;
          if (op == '-')
            left -= r;
          if (op == '*')
            left *= r;
          if (op == '/') {
            if (r != 0)
              left /= r;
            else
              left = 0;
          }

          std::string res = std::to_string(left);
          res.erase(res.find_last_not_of('0') + 1, std::string::npos);
          if (res.back() == '.')
            res.pop_back();

          disp = res;
          op = 0;
          typing = false;
        } else if (b.label == "+" || b.label == "-" || b.label == "*" ||
                   b.label == "/") {

          try {
            left = std::stod(disp);
          } catch (...) {
            if (op == 0)
              left = 0;
          }

          op = b.label[0];

          std::string op1 = std::to_string(left);
          op1.erase(op1.find_last_not_of('0') + 1, std::string::npos);
          if (op1.back() == '.')
            op1.pop_back();

          disp = op1 + " " + b.label;

          typing = false;
        } else if (b.label == ".") {
          if (disp.find('.') == std::string::npos) {
            if (!typing) {
              if (op != 0) {
                size_t op_pos = disp.find(op);
                if (op_pos != std::string::npos) {
                  disp.erase(op_pos + 2);
                } else {
                  disp = "0";
                }
              }
              disp = "0";
            }
            disp += ".";
            typing = true;
          }
        } else {
          if (!typing || disp == "0") {
            if (op != 0) {
              disp = "";
            } else {
              disp = "";
            }
          }

          disp += b.label;
          typing = true;

          if (op != 0) {
            disp = b.label;
          }
        }
      }
    }
    renderer.drawText(font, prog, disp, 25, 65, white);
    renderer.drawText(fontAppln, prog, appln, 130, 10, white);

    glfwSwapBuffers(win);
  }

  glfwTerminate();
  return 0;
}
