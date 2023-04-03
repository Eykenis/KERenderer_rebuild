#include "BlinnShader.h"


BlinnShader::BlinnShader(Mesh* m, float _gloss) {
    mesh = m;
    gloss = _gloss;
    Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
    Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
    Kd = kmath::vec3f(1.f, 1.f, 1.f);
}
void BlinnShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[mesh->face[face][nface].z], 0.);
    vec = model * vec;
    if (nface == 0) worldz.x = vec.z - cameraPos.z;
    if (nface == 1) worldz.y = vec.z - cameraPos.z;
    if (nface == 2) worldz.z = vec.z - cameraPos.z;
    vec = proj * view * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];
}
bool BlinnShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
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
    kmath::vec3f norm;
    kmath::vec3f fragPos = kmath::normalize(v1.xyz * bary.x + v2.xyz * bary.y + v3.xyz * bary.z);
    if (!mesh->normal_map)
        norm = kmath::normalize(n1 * bary.x + n2 * bary.y + n3 * bary.z);
    else {
        TGAcolor nm_ref = mesh->normal_map->get(tex_u * mesh->normal_map->getWidth(), (1 - tex_v) * mesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        kmath::vec4f _norm = kmath::vec4f(norm, 0.f);
        _norm = kmath::normalize(model.inverse().transpose() * _norm);
        norm = _norm.xyz;
    }
    kmath::vec3f diff, spec, ambi;
    if (mesh->diffuse) {
        TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        kmath::vec3f lightDir = kmath::normalize(lightPos - fragPos) * lightIntensity;
        kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
        spec = (prod(lightColor, Ks)) * pow(max(0, norm * halfDir), gloss);
        //diff = prod(Kd, diff) * (floor((max(norm * lightPos, 0.f) * 4.)) / 4.); // Ramp
        diff = prod(Kd, diff) * max(norm * lightDir, 0.f);
        ambi = prod(lightColor, Ka);
        color = (spec + diff + ambi);
    }
    else color = lightColor * (kmath::normalize(lightPos - fragPos) * lightIntensity * norm);
    cut_to_0_255(color);
    return true;
}