#include "PhongShader.h"

PhongShader::PhongShader(Mesh* m) {
    mesh = m;
}
void PhongShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[mesh->face[face][nface].z], 0.);
    vec = proj * view * model * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];
}
bool PhongShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    kmath::vec3f norm = n1 * bary.x + n2 * bary.y + n3 * bary.z;
    kmath::vec3f diff;
    if (mesh->diffuse) {
        float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
        float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z; TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        color = diff * max((lightPos * norm), 0.f);
    }
    else color = lightColor * (lightPos * norm);
    cut_to_0_255(color);
    return true;
}