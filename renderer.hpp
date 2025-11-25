#pragma once
#include "fontLoader.hpp"
#include "glad/glad.h"
#include <string>

class Renderer {
public:
  Renderer();
  ~Renderer();

  void init();

  void setProjection(GLuint prog, int width, int height);

  void drawRect(GLuint prog, float x, float y, float w, float h,
                const float c[4]);

  void drawText(Font &font, GLuint prog, const std::string &s, float x, float y,
                const float c[4]);

private:
  GLuint VAO, VBO, EBO;

  void updateQuad(float x, float y, float w, float h, float u0, float v0,
                  float u1, float v1);
};
