#include "TexShader.h"

TexShader::TexShader(Mesh* m) : Shader(m) { }
void TexShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    vec = model * vec;
    worldPos[nface] = vec;
    vec = proj * view * vec;
    pos[nface] = vec;
    uv[nface] = mesh->tex_coord[smesh->face[face][nface].y];
}
bool TexShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    kmath::vec3f diff;
    if (smesh->diffuse) {
        float tex_u = uv[0].x * bary.x + uv[1].x * bary.y + uv[2].x * bary.z;
        float tex_v = uv[0].y * bary.x + uv[1].y * bary.y + uv[2].y * bary.z;

        TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
        //if (ref.a < 0.1) return false;
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        color = diff;
    }
    else color = smesh->Kd * 255;
    cut_to_0_255(color);
    return true;
}