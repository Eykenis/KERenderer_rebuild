#include "RampShader.h"

void RampShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    vec = proj * view * model * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[smesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[smesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[smesh->face[face][2].y];
}
bool RampShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
    float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
    kmath::vec3f norm;
    if (!smesh->normal_map)
        norm = kmath::normalize(n1 * bary.x + n2 * bary.y + n3 * bary.z);
    else {
        TGAcolor nm_ref = smesh->normal_map->get(tex_u * smesh->normal_map->getWidth(), (1 - tex_v) * smesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        norm = kmath::normalize(model.inverse().transpose() * kmath::vec4f(norm, 0.f)).xyz;
    }
    kmath::vec3f diff, ambi;
    kmath::vec3f fragPos = (viewport.inverse() * (v1 * bary.x + v2 * bary.y + v3 * bary.z)).xyz;
    kmath::vec3f lightDir = kmath::normalize(lightPos - fragPos) * lightIntensity;
    if (smesh->diffuse) {
        TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
    }
    else diff = smesh->Kd * 255;
    if (ramp) {
        TGAcolor ref = ramp->get((1 - (max(norm * lightDir, 0.f)) * 0.8 + 0.2) * ramp->getWidth(), 1);
        for (int i = 0; i < 3; ++i) smesh->Kd.v[i] = ref.raw[i] / 255.;
    }
    else smesh->Kd = kmath::vec3f(1, 1, 1);
    ambi = prod(ambientColor, smesh->Ka);
    color = prod(prod(smesh->Kd, diff), lightColor) + ambi;
    cut_to_0_255(color);
    return true;
}