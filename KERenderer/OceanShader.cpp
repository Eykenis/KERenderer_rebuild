#include "OceanShader.h"

void OceanShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    vec = model * vec;
    worldPos[nface] = vec;
    vec = proj * view * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    pos[nface] = vec;
    uv[nface] = mesh->tex_coord[smesh->face[face][nface].y];
    worldNormal[nface] = norm.xyz;
}
bool OceanShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv[0].x * bary.x + uv[1].x * bary.y + uv[2].x * bary.z;
    float tex_v = uv[0].y * bary.x + uv[1].y * bary.y + uv[2].y * bary.z;

    kmath::vec3f norm;
    kmath::vec3f fragPos = (worldPos[0] * bary.x + worldPos[1] * bary.y + worldPos[2] * bary.z).xyz;
    if (!smesh->normal_map)
        norm = kmath::normalize(worldNormal[0] * bary.x + worldNormal[1] * bary.y + worldNormal[2] * bary.z);
    else {
        TGAcolor nm_ref = smesh->normal_map->get(tex_u * smesh->normal_map->getWidth(), (1 - tex_v) * smesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        norm = kmath::normalize(model.inverse().transpose() * kmath::vec4f(norm, 0.f)).xyz;
    }
    kmath::vec3f diff, spec, ambi;
    if (smesh->diffuse) {
        TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        diff = prod(smesh->Kd, diff);
    }
    else diff = prod(smesh->Kd, kmath::vec3f(15, 104, 189));
    kmath::vec3f lightDir = kmath::normalize(lightPos - fragPos) * lightIntensity;
    kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
    spec = prod(prod(lightColor, smesh->Ks), diff) * pow(max(0, norm * halfDir), gloss);
    diff = prod(diff * max(norm * lightDir * 0.2 + 0.8, 0.f), lightColor);
    ambi = prod(ambientColor, smesh->Ka);
    color = (spec + diff + ambi);
    cut_to_0_255(color);
    return true;
}