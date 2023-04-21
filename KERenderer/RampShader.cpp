#include "RampShader.h"

void RampShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    vec = proj * view * model * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    pos[nface] = vec;
    uv[nface] = mesh->tex_coord[smesh->face[face][nface].y];
    worldNormal[nface] = norm.xyz;
}
bool RampShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv[0].x * bary.x + uv[1].x * bary.y + uv[2].x * bary.z;
    float tex_v = uv[0].y * bary.x + uv[1].y * bary.y + uv[2].y * bary.z;
    kmath::vec3f norm;
    if (!smesh->normal_map)
        norm = kmath::normalize(worldNormal[0] * bary.x + worldNormal[1] * bary.y + worldNormal[2] * bary.z);
    else {
        TGAcolor nm_ref = smesh->normal_map->get(tex_u * smesh->normal_map->getWidth(), (1 - tex_v) * smesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        norm = kmath::normalize(model.inverse().transpose() * kmath::vec4f(norm, 0.f)).xyz;
    }
    kmath::vec3f diff, ambi;
    kmath::vec3f fragPos = (viewport.inverse() * (pos[0] * bary.x + pos[1] * bary.y + pos[2] * bary.z)).xyz;
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