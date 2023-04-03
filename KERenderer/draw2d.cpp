#include "draw2d.h"

extern float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

void drawLine(unsigned char *framebuffer, int x0, int y0, int x1, int y1, kmath::vec3f color) {
    // Bresenham
    y0 = WINDOW_HEIGHT - y0, y1 = WINDOW_HEIGHT - y1;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int gx = x0 < x1 ? 1 : -1;
    int gy = y0 < y1 ? 1 : -1;
    int p = (dx > dy ? dx : -dy) / 2;
    while (!(x0 == x1 && y0 == y1)) {
        drawpixel(framebuffer, x0, WINDOW_HEIGHT - y0, color);
        int tmp = p;
        if (tmp > -dx) p -= dy, x0 += gx;
        if (tmp < dy)  p += dx, y0 += gy;
    }
}

bool inTriangle(kmath::vec3f v) {
    return v.x >= 0. && v.y >= 0. && v.z >= 0.;
}

kmath::vec3f barycentric(kmath::vec2f p, kmath::vec2f a, kmath::vec2f b, kmath::vec2f c) {
    kmath::vec2f v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0 * v0;
    float d01 = v0 * v1;
    float d11 = v1 * v1;
    float d20 = v2 * v0;
    float d21 = v2 * v1;
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    return kmath::vec3f(1.0f - v - w, v, w);
}


void drawpixel(unsigned char* framebuffer, int x, int y, kmath::vec3f color) {
    int idx = (y * WINDOW_WIDTH + x) * 4;
    for (int i = 0; i < 3; ++i) framebuffer[idx + i] = color.v[i];
}

void intensity(unsigned char* framebuffer, int x, int y, kmath::vec3f intense) {
    int idx = (y * WINDOW_WIDTH + x) * 4;
    for (int i = 0; i < 3; ++i) framebuffer[idx + i] = round(framebuffer[idx + i] * intense.v[i]);
}

kmath::vec3f getpixel(unsigned char* buffer, int x, int y) {
    kmath::vec3f ret;
    int idx = (y * WINDOW_WIDTH + x) * 4;
    for (int i = 0; i < 3; ++i) ret.v[i] = buffer[idx + i];
    return ret;
}

void cut_to_0_255(kmath::vec3f& vec) {
    for (int i = 0; i < 3; ++i) {
        vec.v[i] = min(vec.v[i], 255);
        vec.v[i] = max(vec.v[i], 0);
    }
}

float doInterpolate(kmath::vec3f interpolate, float z1, float z2, float z3) {
    return interpolate.x * z1 + interpolate.y * z2 + interpolate.z * z3;
}