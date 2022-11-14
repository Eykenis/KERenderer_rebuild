#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;
extern kmath::vec3f cameraFront;

class ColorShader :
    public Shader
{
private:
    kmath::vec3f _color;
public:
    ColorShader(Mesh* m, kmath::vec3f c) {
        mesh = m;
        _color = c;
    }
    void vert(int face, int nface) {
        kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
        vec = viewport * proj * view * model * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        if (nface == 0) v1 = vec.xyz;
        else if (nface == 1) v2 = vec.xyz;
        else if (nface == 2) v3 = vec.xyz;
    }
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) {
        color = _color;
        cut_to_0_255(color);
        return true;
    }
};

