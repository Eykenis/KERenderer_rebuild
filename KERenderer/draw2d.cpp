#include "draw2d.h"
#include "api.h"
#include "macro.h"

extern float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

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

void cut_to_0_255(kmath::vec3f &vec) {
    for (int i = 0; i < 3; ++i) {
        vec.v[i] = min(vec.v[i], 255);
        vec.v[i] = max(vec.v[i], 0);
    }
}

void drawTriangle(kmath::vec3f v1, kmath::vec3f v2, kmath::vec3f v3, kmath::vec3f n1, kmath::vec3f n2, kmath::vec3f n3) {
    float xl = min(v1.x, min(v2.x, v3.x)), xr = max(v1.x, max(v2.x, v3.x));
    float yd = min(v1.y, min(v2.y, v3.y)), yu = max(v1.y, max(v2.y, v3.y));
    float z;
    for (int i = xl; i <= xr; ++i) {
        if (i < 0 || i > WINDOW_WIDTH) continue;
        for (int j = yd; j <= yu; ++j) {
            if (j < 0 || j > WINDOW_HEIGHT) continue;
            kmath::vec3f interpolate = barycentric(kmath::vec2f(i, j), kmath::vec2f(v1.x, v1.y), kmath::vec2f(v2.x, v2.y), kmath::vec2f(v3.x, v3.y));
            if (inTriangle(interpolate) && (z = doInterpolate(interpolate, v1.z, v2.z, v3.z)) > zbuffer[i][j]) {
                zbuffer[i][j] = z;
                kmath::vec3f norm = n1 * interpolate.x + n2 * interpolate.y + n3 * interpolate.z;
                kmath::vec3f color = lightColor * (lightDir * norm);
                cut_to_0_255(color);
                putpixel(i, j, (unsigned)color.b << 16 | (unsigned)color.g << 8 | (unsigned)color.r);
            }
        }
    }
}
