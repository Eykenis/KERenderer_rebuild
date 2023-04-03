#pragma once
#include "kmath.h"
#include "api.h"
#include "macro.h"

void drawLine(unsigned char* framebuffer, int x0, int y0, int x1, int y1, kmath::vec3f color);
bool inTriangle(kmath::vec3f v);
kmath::vec3f barycentric(kmath::vec2f p, kmath::vec2f a, kmath::vec2f b, kmath::vec2f c);
void drawTriangle(float xa, float ya, float xb, float yb, float xc, float yc, int c1, int c2, int c3);
float doInterpolate(kmath::vec3f interpolate, float z1, float z2, float z3);
void cut_to_0_255(kmath::vec3f &vec);
void drawpixel(unsigned char* framebuffer, int x, int y, kmath::vec3f color);
kmath::vec3f getpixel(unsigned char* buffer, int x, int y);
void intensity(unsigned char* framebuffer, int x, int y, kmath::vec3f intense);
