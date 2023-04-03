#include "RampShader.h"

RampShader::RampShader(Mesh* m, const char* ramp_name) {
    mesh = m;
    Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
    Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
    Kd = kmath::vec3f(1.f, 1.f, 1.f);
    ramp = NULL;
    if (ramp_name) {
        ramp = new TGAimage;
        ramp->read_TGA(ramp_name);
    }
}
void RampShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[mesh->face[face][nface].z], 0.);
    vec = proj * view * model * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];
}
bool RampShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
    float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
    kmath::vec3f norm;
    if (!mesh->normal_map)
        norm = kmath::normalize(n1 * bary.x + n2 * bary.y + n3 * bary.z);
    else {
        TGAcolor nm_ref = mesh->normal_map->get(tex_u * mesh->normal_map->getWidth(), (1 - tex_v) * mesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        kmath::vec4f _norm = kmath::vec4f(norm, 0.f);
        _norm = kmath::normalize(model.inverse().transpose() * _norm);
        norm = _norm.xyz;
    }
    kmath::vec3f diff, ambi;
    if (mesh->diffuse) {
        TGAcolor ref = ramp->get((1 - (max(norm * lightPos, 0.f) * 0.8 + 0.2)) * ramp->getWidth(), 1);
        for (int i = 0; i < 3; ++i) Kd.v[i] = ref.raw[i] / 255.;
        ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
        ambi = prod(lightColor, Ka);
        color = prod(Kd, diff) + ambi;
    }
    else color = lightColor * (lightPos * norm);
    cut_to_0_255(color);
    return true;
}