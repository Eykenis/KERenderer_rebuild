#include "draw2d.h"
#include "api.h"
#include "macro.h"

void drawLine(int x0, int y0, int x1, int y1, int color) {
    y0 = WINDOW_HEIGHT - y0, y1 = WINDOW_HEIGHT - y1;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int gx = x0 < x1 ? 1 : -1;
    int gy = y0 < y1 ? 1 : -1;
    int p = (dx > dy ? dx : -dy) / 2;
    while (!(x0 == x1 && y0 == y1)) {
        putpixel(x0, WINDOW_HEIGHT - y0, color);
        int tmp = p;
        if (tmp > -dx) p -= dy, x0 += gx;
        if (tmp < dy)  p += dx, y0 += gy;
    }
}

bool inTriangle(kmath::vec3f v) {
    return v.x >= 0. && v.y >= 0. && v.z >= 0.;
}

bool onTriangle(kmath::vec3f v) {
    return abs(v.x) < 0.02 || abs(v.y) < 0.02 || abs(v.z) < 0.02;
}

void lineTriangle(float xa, float ya, float xb, float yb, float xc, float yc) {
    drawLine(xa, ya, xb, yb, 0xffffff);
    drawLine(xa, ya, xc, yc, 0xffffff);
    drawLine(xb, yb, xc, yc, 0xffffff);
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

float doInterpolate(kmath::vec3f interpolate, float z1, float z2, float z3) {
    return interpolate.x * z1 + interpolate.y * z2 + interpolate.z * z3;
}

void cut_to_0_255(kmath::vec3f& vec) {
    if (vec.x > 255.) vec.x = 255.;
    if (vec.y > 255.) vec.y = 255.;
    if (vec.z > 255.) vec.z = 255.;
    if (vec.x < 0.) vec.x = 0.;
    if (vec.x < 0.) vec.y = 0.;
    if (vec.x < 0.) vec.z = 0.;
}