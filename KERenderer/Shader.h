#pragma once
#include "Mesh.h"
#include "draw2d.h"

extern float zbuffer[WINDOW_WIDTH][WINDOW_HEIGHT];
extern unsigned char* framebuffer;

class Shader
{
protected:
   Mesh* mesh;
   kmath::vec3f v1, v2, v3;
public:
	virtual void vert(kmath::vec3i face, int nface) = 0;
	virtual void frag(kmath::vec3f& bary, kmath::vec3f& color, int nface) = 0;

    void work() {
        for (int i = 0; i < WINDOW_WIDTH; ++i) {
            for (int j = 0; j < WINDOW_HEIGHT; ++j) {
                zbuffer[i][j] = -1e10;
            }
        } // clear z buffer

        for (int k = 0; k < mesh->face.size(); ++k) {
            kmath::vec3f normal = kmath::cross((v3 - v2), (v2 - v1));
            vert(mesh->face[k][0], 0);
            vert(mesh->face[k][1], 1);
            vert(mesh->face[k][2], 2);
            float xl = min(v1.x, min(v2.x, v3.x)), xr = max(v1.x, max(v2.x, v3.x));
            float yd = min(v1.y, min(v2.y, v3.y)), yu = max(v1.y, max(v2.y, v3.y));
            float z;
            for (int i = xl; i <= xr; ++i) {
                if (i <= 0 || i >= WINDOW_WIDTH) continue;
                for (int j = yd; j <= yu; ++j) {
                    if (j <= 0 || j >= WINDOW_HEIGHT) continue;
                    kmath::vec3f interpolate = barycentric(kmath::vec2f(i, j), kmath::vec2f(v1.x, v1.y), kmath::vec2f(v2.x, v2.y), kmath::vec2f(v3.x, v3.y));
                    if (inTriangle(interpolate) && (z = doInterpolate(interpolate, v1.z, v2.z, v3.z)) > zbuffer[i][j]) {
                        zbuffer[i][j] = z;
                        kmath::vec3f color;
                        frag(interpolate, color, k);
                        drawpixel(framebuffer, i, j, color);
                    }
                }
            }
        }
    }
};