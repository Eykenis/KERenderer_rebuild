#pragma once
#include "kmath.h"

void drawLine(int x0, int y0, int x1, int y1, int color);
bool inTriangle(kmath::vec3f v);
void lineTriangle(float xa, float ya, float xb, float yb, float xc, float yc);
kmath::vec3f barycentric(kmath::vec2f p, kmath::vec2f a, kmath::vec2f b, kmath::vec2f c);
void drawTriangle(float xa, float ya, float xb, float yb, float xc, float yc, int c1, int c2, int c3);