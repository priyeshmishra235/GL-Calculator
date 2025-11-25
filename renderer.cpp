#include "renderer.hpp"

Renderer::Renderer() : VAO(0), VBO(0), EBO(0) {}

Renderer::~Renderer() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Renderer::init() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, nullptr, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Renderer::updateQuad(float x, float y, float w, float h, float u0,
                          float v0, float u1, float v1) {
  float verts[] = {x,     y,     u0, v0, x + w, y,     u1, v0,
                   x + w, y + h, u1, v1, x,     y + h, u0, v1};
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
}

void Renderer::setProjection(GLuint prog, int w, int h) {
  glUseProgram(prog);
  float L = 0, R = (float)w, T = 0, B = (float)h;
  float ortho[16] = {2.0f / (R - L),
                     0.0f,
                     0.0f,
                     0.0f,
                     0.0f,
                     2.0f / (T - B),
                     0.0f,
                     0.0f,
                     0.0f,
                     0.0f,
                     -1.0f,
                     0.0f,
                     -(R + L) / (R - L),
                     -(T + B) / (T - B),
                     0.0f,
                     1.0f};
  glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE,
                     ortho);
}

void Renderer::drawRect(GLuint prog, float x, float y, float w, float h,
                        const float c[4]) {
  glUseProgram(prog);
  glUniform4fv(glGetUniformLocation(prog, "color"), 1, c);
  glUniform1i(glGetUniformLocation(prog, "isText"), 0);

  glBindVertexArray(VAO);
  updateQuad(x, y, w, h, 0, 0, 1, 1);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::drawText(Font &font, GLuint prog, const std::string &s, float x,
                        float y, const float c[4]) {
  glUseProgram(prog);
  glUniform4fv(glGetUniformLocation(prog, "color"), 1, c);
  glUniform1i(glGetUniformLocation(prog, "isText"), 1);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font.tex);
  glBindVertexArray(VAO);

  float penX = x;
  float penY = y + font.ascent;

  for (char ch : s) {
    if (ch < 32 || ch > 127)
      continue;
    Glyph &g = font.glyphs[ch];

    float w = g.x1 - g.x0;
    float h = g.y1 - g.y0;

    updateQuad(penX + g.x0, penY + g.y0, w, h, g.u0, g.v0, g.u1, g.v1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    penX += g.xAdvance;
  }
}
