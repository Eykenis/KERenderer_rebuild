#include "TexShader.h"

TexShader::TexShader(Mesh* m) {
    mesh = m;
}
void TexShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    vec = model * vec;

    if (nface == 0) worldz.x = vec.z - cameraPos.z;
    if (nface == 1) worldz.y = vec.z - cameraPos.z;
    if (nface == 2) worldz.z = vec.z - cameraPos.z;

    vec = proj * view * vec;

    if (nface == 0) v1 = vec, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, uv3 = mesh->tex_coord[mesh->face[face][2].y];
}
bool TexShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    kmath::vec3f diff;
    if (mesh->diffuse) {

        kmath::vec3f bary_r(bary);
        for (int i = 0; i < 3; ++i) {
            bary_r.v[i] = bary.v[i] / worldz.v[i];
        }
        float zn = 1. / (bary_r.x + bary_r.y + bary_r.z);
        for (int i = 0; i < 3; ++i) {
            bary_r.v[i] *= zn;
        }
        float tex_u = uv1.x * bary_r.x + uv2.x * bary_r.y + uv3.x * bary_r.z;
        float tex_v = uv1.y * bary_r.x + uv2.y * bary_r.y + uv3.y * bary_r.z;

        TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
        //if (ref.a < 0.1) return false;
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        color = diff;
    }
    cut_to_0_255(color);
    return true;
}