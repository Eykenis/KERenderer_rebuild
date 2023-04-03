#include "ShadowShader.h"

ShadowShader::ShadowShader(Mesh* m) {
    mesh = m;
}

void ShadowShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    vec = proj * view * model * vec;
    vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
    vec.w /= vec.w;
    if (nface == 0) v1 = vec, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, uv3 = mesh->tex_coord[mesh->face[face][2].y];
}
bool ShadowShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    color = kmath::vec3f(255., 255., 255);
    return false;
}