#include "shader.hpp"
#include <iostream>

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D text;
    uniform vec4 color;
    uniform bool isText;

    void main() {
        if (isText) {
            float sampled = texture(text, TexCoord).r;
            FragColor = vec4(color.rgb, sampled * color.a);
        } else {
            FragColor = color;
        }
    }
)";

GLuint Shader::compile(GLenum type, const char *src) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, nullptr);
  glCompileShader(s);
  int success;
  glGetShaderiv(s, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(s, 512, nullptr, infoLog);
    std::cerr << "Shader Error: " << infoLog << std::endl;
  }
  return s;
}

GLuint Shader::createProgram() {
  GLuint v = compile(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint f = compile(GL_FRAGMENT_SHADER, fragmentShaderSource);
  GLuint p = glCreateProgram();
  glAttachShader(p, v);
  glAttachShader(p, f);
  glLinkProgram(p);
  glDeleteShader(v);
  glDeleteShader(f);
  return p;
}
