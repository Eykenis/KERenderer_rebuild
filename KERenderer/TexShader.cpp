#include "TexShader.h"

TexShader::TexShader(Mesh* m) : Shader(m) { }
void TexShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    vec = model * vec;
    worldPos[nface] = vec;
    vec = proj * view * vec;

    if (nface == 0) v1 = vec, uv1 = mesh->tex_coord[smesh->face[face][0].y];
    else if (nface == 1) v2 = vec, uv2 = mesh->tex_coord[smesh->face[face][1].y];
    else if (nface == 2) v3 = vec, uv3 = mesh->tex_coord[smesh->face[face][2].y];
}
bool TexShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    kmath::vec3f diff;
    if (smesh->diffuse) {
        //kmath::vec3f bary_r(bary);
        //for (int i = 0; i < 3; ++i) {
        //    bary_r.v[i] = bary.v[i] / worldz.v[i];
        //}
        //float zn = 1. / (bary_r.x + bary_r.y + bary_r.z);
        //for (int i = 0; i < 3; ++i) {
        //    bary_r.v[i] *= zn;
        //}
        float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
        float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;

        TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
        //if (ref.a < 0.1) return false;
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        color = diff;
    }
    else color = smesh->Kd * 255;
    cut_to_0_255(color);
    return true;
}