#pragma once
#include "glad/glad.h"

class Shader {
public:
  static GLuint createProgram();

private:
  static GLuint compile(GLenum type, const char *src);
};
