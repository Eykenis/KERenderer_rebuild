#include "BlinnShader_tangent.h"

BlinnShader_tangent::BlinnShader_tangent(Mesh* m, float _gloss) {
    mesh = m;
    gloss = _gloss;
    Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
    Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
    Kd = kmath::vec3f(1.f, 1.f, 1.f);
}
void BlinnShader_tangent::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    vec = model * vec;
    worldz.v[nface] = vec.z;
    vec = proj * view * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz;
    else if (nface == 1) v2 = vec, n2 = norm.xyz;
    else if (nface == 2) v3 = vec, n3 = norm.xyz;
    kmath::vec3f N = kmath::normalize(norm.xyz);
    kmath::vec3f T = kmath::normalize(t - N * (t * N));
    kmath::vec3f B = kmath::normalize(cross(N, T));
    TBN = kmath::mat4f::identical();
    for (int i = 0; i < 3; ++i) TBN.m[0][i] = T.v[i], TBN.m[1][i] = B.v[i], TBN.m[2][i] = N.v[i];
    kmath::vec4f vs[] = { v1, v2, v3 };
    t_lightDir[nface] = kmath::normalize((TBN * kmath::vec4f(lightPos, 0.f)).xyz);
    t_cameraFront[nface] = kmath::normalize((TBN * kmath::vec4f(cameraFront, 0.f)).xyz);
}
bool BlinnShader_tangent::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
    float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
    kmath::vec3f _lightDir = t_lightDir[0] * bary.x + t_lightDir[1] * bary.y + t_lightDir[2] * bary.z;
    kmath::vec3f _cameraFront = t_cameraFront[0] * bary.x + t_cameraFront[1] * bary.y + t_cameraFront[2] * bary.z;
    _lightDir = kmath::normalize(_lightDir), _cameraFront = kmath::normalize(_cameraFront);
    kmath::vec3f norm;
    TGAcolor nm_ref = smesh->normal_map->get(tex_u * smesh->normal_map->getWidth(), (1 - tex_v) * smesh->normal_map->getHeight());
    for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
    norm = kmath::normalize(kmath::vec4f(norm, 0.f)).xyz;
    kmath::vec3f diff, spec, ambi;
    TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
    for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
    kmath::vec3f halfDir = kmath::normalize(_lightDir - _cameraFront);
    spec = (prod(lightColor, Ks)) * pow(max(0, norm * halfDir), gloss);
    diff = (prod(Kd, diff)) * (max(norm * _lightDir, 0.f));
    ambi = prod(lightColor, Ka);
    color = diff + ambi + spec;
    cut_to_0_255(color);
    return true;
}