#pragma once
#include "Shader.h"

extern kmath::mat4f lightSpaceMatrix, viewport, model;
extern float* zbuffer, *shadowbuffer;
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

class SimpleDepthShader :
    public Shader
{
public:
    SimpleDepthShader(Mesh* m) {
        mesh = m;
    }
private:
    void vert(int face, int nface) {
        kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
        vec = (viewport * lightSpaceMatrix * model) * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        if (nface == 0) v1 = vec.xyz;
        else if (nface == 1) v2 = vec.xyz;
        else if (nface == 2) v3 = vec.xyz;
    }
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) {
        color = kmath::vec3f(255., 255., 255) * (doInterpolate(bary, v1.z, v2.z, v3.z) / 2.f);
        cut_to_0_255(color);
        return false;
    }
};

