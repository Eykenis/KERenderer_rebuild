#pragma once
#include "Shader.h"

class PhongShader :
    public Shader
{
private:
    kmath::vec3f n1, n2, n3;
public:
    PhongShader(Mesh* m) {
        mesh = m;
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
        kmath::vec3f norm = n1 * bary.x + n2 * bary.y + n3 * bary.z;
        kmath::vec3f diff;
        if (mesh->diffuse) {
            SetWorkingImage(mesh->diffuse);
            float tex_u = mesh->tex_coord[mesh->face[nface][0].y].x * bary.x + mesh->tex_coord[mesh->face[nface][1].y].x * bary.y + mesh->tex_coord[mesh->face[nface][2].y].x * bary.z;
            float tex_v = mesh->tex_coord[mesh->face[nface][0].y].y * bary.x + mesh->tex_coord[mesh->face[nface][1].y].y * bary.y + mesh->tex_coord[mesh->face[nface][2].y].y * bary.z;
            COLORREF ref = getpixel(tex_u * mesh->diffuse->getwidth(), (1 - tex_v) * mesh->diffuse->getheight());
            diff.b = ((ref >> 16) & 0xff) * lightColor.b / 255;
            diff.g = ((ref >> 8) & 0xff) * lightColor.g / 255;
            diff.r = ((ref & 0xff) & 0xff) * lightColor.r / 255;
            color = diff * (lightDir * norm);
            SetWorkingImage();
        }
        else color = lightColor * (lightDir * norm);
        cut_to_0_255(color);
    }
};

