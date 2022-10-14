#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern float zbuffer[WINDOW_WIDTH][WINDOW_HEIGHT];
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;
extern kmath::vec3f cameraFront;

class BlinnShader :
    public Shader
{
private:
    kmath::vec3f n1, n2, n3;
    kmath::vec3f Ks, Kd, Ka;
public:
    float gloss;
    BlinnShader(Mesh* m, float _gloss = 100.) {
        mesh = m;
        gloss = _gloss;
        Ks = kmath::vec3f(1.f, 1.f, 1.f);
        Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
        Kd = kmath::vec3f(1.f, 1.f, 1.f);
    }
    void vert(kmath::vec3i face, int nface) {
        kmath::vec4f vec(mesh->vert[face.x], 1.);
        kmath::vec4f norm(mesh->normal[face.z], 0.);
        vec = viewport * proj * view * model * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        norm = kmath::normalize(model * norm);
        if (nface == 0) v1 = vec.xyz, n1 = norm.xyz;
        else if (nface == 1) v2 = vec.xyz, n2 = norm.xyz;
        else if (nface == 2) v3 = vec.xyz, n3 = norm.xyz;
    }
    void frag(kmath::vec3f& bary, kmath::vec3f& color, int nface) {
        kmath::vec3f norm = kmath::normalize(n1 * bary.x + n2 * bary.y + n3 * bary.z);
        kmath::vec3f diff, spec, ambi;
        if (mesh->diffuse) {
            float tex_u = mesh->tex_coord[mesh->face[nface][0].y].x * bary.x + mesh->tex_coord[mesh->face[nface][1].y].x * bary.y + mesh->tex_coord[mesh->face[nface][2].y].x * bary.z;
            float tex_v = mesh->tex_coord[mesh->face[nface][0].y].y * bary.x + mesh->tex_coord[mesh->face[nface][1].y].y * bary.y + mesh->tex_coord[mesh->face[nface][2].y].y * bary.z;
            TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
            for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
            kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
            spec = (prod(lightColor, Ks)) * pow(max(0, norm * halfDir), gloss);
            diff = (prod(Kd, diff)) * ((lightDir * norm));
            ambi = prod(lightColor, Ka);
            color = diff + spec + ambi;
        }
        else color = lightColor * (lightDir * norm);
        cut_to_0_255(color);
    }
};

