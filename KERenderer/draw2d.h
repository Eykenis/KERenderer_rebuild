#pragma once
#include "kmath.h"

void drawLine(int x0, int y0, int x1, int y1, int color);
bool inTriangle(kmath::vec3f v);
void lineTriangle(float xa, float ya, float xb, float yb, float xc, float yc);
kmath::vec3f barycentric(kmath::vec2f p, kmath::vec2f a, kmath::vec2f b, kmath::vec2f c);
float doInterpolate(kmath::vec3f interpolate, float z1, float z2, float z3);
void cut_to_0_255(kmath::vec3f &vec);
void drawTriangle(kmath::vec3f v1, kmath::vec3f v2, kmath::vec3f v3, kmath::vec3f c1, kmath::vec3f c2, kmath::vec3f c3);