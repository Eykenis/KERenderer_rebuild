#pragma once
#include "api.h"
#include "Mesh.h"
#include "draw2d.h"

extern unsigned char* framebuffer;
extern kmath::vec3f lightDir, cameraFront;
extern unsigned char* stencilbuffer;
extern bool stencil_write, stencil_read;
class Shader
{
protected:
   Mesh* mesh;
   kmath::vec3f v1, v2, v3;
   kmath::vec2f uv1, uv2, uv3;
   kmath::vec3f n1, n2, n3;
   kmath::vec3f t, b, n;
   int i, j;
public:
	virtual void vert(int face, int nface) = 0;
	virtual bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) = 0;

    void work(float* buffer) {
        for (int i = 0; i < WINDOW_WIDTH; ++i) {
            for (int j = 0; j < WINDOW_HEIGHT; ++j) {
                buffer[i * WINDOW_HEIGHT + j] = -1e10;
            }
        } // clear buffer

        // MVP process
        for (int k = 0; k < mesh->face.size(); ++k) {
            uv1 = mesh->tex_coord[mesh->face[k][0].y];
            uv2 = mesh->tex_coord[mesh->face[k][1].y];
            uv3 = mesh->tex_coord[mesh->face[k][2].y];
            kmath::getTBN(t, b, mesh->vert[mesh->face[k][0].x], mesh->vert[mesh->face[k][1].x], mesh->vert[mesh->face[k][2].x], uv1, uv2, uv3);
            vert(k, 0);
            vert(k, 1);
            vert(k, 2);
            kmath::vec3f v21 = v2 - v1;
            kmath::vec3f v32 = v3 - v1;
            kmath::vec3f crs = cross(v21, v32);
            if (cross(v21, v32).z <= 0) continue;
            float xl = min(v1.x, min(v2.x, v3.x)), xr = max(v1.x, max(v2.x, v3.x));
            float yd = min(v1.y, min(v2.y, v3.y)), yu = max(v1.y, max(v2.y, v3.y));
            float z;
            for (i = xl; i <= xr; ++i) {
                if (i <= 0 || i >= WINDOW_WIDTH) continue;
                for (j = yd; j <= yu; ++j) {
                    if (j <= 0 || j >= WINDOW_HEIGHT) continue;
                    kmath::vec3f interpolate = barycentric(kmath::vec2f(i, j), kmath::vec2f(v1.x, v1.y), kmath::vec2f(v2.x, v2.y), kmath::vec2f(v3.x, v3.y));
                    if (inTriangle(interpolate) && (z = doInterpolate(interpolate, v1.z, v2.z, v3.z)) > buffer[i * WINDOW_HEIGHT + j]) {
                        buffer[WINDOW_HEIGHT * i + j] = z;
                        kmath::vec3f color;
                        int idx = (j * WINDOW_HEIGHT + i);
                        if (!stencil_read || (stencil_read && stencilbuffer[idx] != 1)) {
                            if (frag(interpolate, color, k, i, j))
                                drawpixel(framebuffer, i, j, color);
                            if (stencil_write) stencilbuffer[idx] = 1;
                        }
                    }
                }
            }
        }
    }
};