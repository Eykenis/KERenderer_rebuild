#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

class TexShader :
    public Shader
{
public:
    TexShader(Mesh* m) {
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
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface) {
        kmath::vec3f diff;
        if (mesh->diffuse) {
            float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
            float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
            TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
            for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
            color = diff;
        }
        cut_to_0_255(color);
        return true;
    }
};

