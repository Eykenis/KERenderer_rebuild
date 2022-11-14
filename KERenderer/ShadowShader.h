#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern float* zbuffer, shadowbuffer;
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

class ShadowShader :
    public Shader
{
public:
    ShadowShader(Mesh* m) {
        mesh = m;
    }
private:
    void vert(int face, int nface) {
        kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
        vec = viewport * proj * view * model * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        if (nface == 0) v1 = vec.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
        else if (nface == 1) v2 = vec.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
        else if (nface == 2) v3 = vec.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];
    }
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) {
        color = kmath::vec3f(255., 255., 255);
        return false;
    }
};

