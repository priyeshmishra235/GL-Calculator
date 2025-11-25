#pragma once

#include "glad/glad.h"

struct Glyph {
  float u0, v0, u1, v1;
  float x0, y0, x1, y1;
  float xAdvance;
};

struct Font {
  GLuint tex;
  float size;
  Glyph glyphs[128];
  float ascent;
  float descent;
};

struct Rect {
  float x, y, w, h;
};

Font loadFont(const char *path, float pixelSize);
