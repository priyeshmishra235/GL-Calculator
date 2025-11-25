#include "fontLoader.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

Font loadFont(const char *path, float pixelSize) {
  Font f = {};
  f.size = pixelSize;

  FILE *file = fopen(path, "rb");
  if (!file) {
    std::cerr << "Failed to load font: " << path << std::endl;
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  std::vector<unsigned char> buffer(size);
  fread(buffer.data(), 1, size, file);
  fclose(file);

  stbtt_fontinfo info;
  if (!stbtt_InitFont(&info, buffer.data(), 0)) {
    std::cerr << "Failed to init font info" << std::endl;
    exit(1);
  }

  float scale = stbtt_ScaleForPixelHeight(&info, pixelSize);

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
  f.ascent = ascent * scale;
  f.descent = descent * scale;

  int atlasW = 512;
  int atlasH = 512;
  std::vector<unsigned char> atlas(atlasW * atlasH, 0);

  int x = 1;
  int y = 1;
  int rowH = 0;

  for (int c = 32; c < 128; c++) {
    int w, h, xoff, yoff;
    unsigned char *bmp =
        stbtt_GetCodepointBitmap(&info, 0, scale, c, &w, &h, &xoff, &yoff);

    if (x + w + 1 > atlasW) {
      x = 1;
      y += rowH + 1;
      rowH = 0;
    }

    for (int j = 0; j < h; j++) {
      for (int i = 0; i < w; i++) {
        atlas[(y + j) * atlasW + (x + i)] = bmp[j * w + i];
      }
    }

    Glyph &g = f.glyphs[c];
    g.u0 = x / (float)atlasW;
    g.v0 = y / (float)atlasH;
    g.u1 = (x + w) / (float)atlasW;
    g.v1 = (y + h) / (float)atlasH;
    g.x0 = (float)xoff;
    g.y0 = (float)yoff;
    g.x1 = (float)(xoff + w);
    g.y1 = (float)(yoff + h);

    int ax, lsb;
    stbtt_GetCodepointHMetrics(&info, c, &ax, &lsb);
    g.xAdvance = ax * scale;

    stbtt_FreeBitmap(bmp, nullptr);

    x += w + 1;
    if (h > rowH)
      rowH = h;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &f.tex);
  glBindTexture(GL_TEXTURE_2D, f.tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasW, atlasH, 0, GL_RED,
               GL_UNSIGNED_BYTE, atlas.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return f;
}
