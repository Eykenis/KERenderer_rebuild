#include "BlinnShader_shadow.h"

void BlinnShader_shadow::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    //kmath::mat4f m = viewport * proj * view * model;
    vec = model * vec;
    worldPos[nface] = vec;
    vec = proj * view * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[smesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[smesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[smesh->face[face][2].y];

    vec = kmath::vec4f(mesh->vert[smesh->face[face][nface].x], 1.f);
    vec = (lightSpaceMatrix * model) * vec;
    if (nface == 0) lv1 = vec; else if (nface == 1) lv2 = vec; else if (nface == 2) lv3 = vec;
}
bool BlinnShader_shadow::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
    float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
    kmath::vec3f norm;
    kmath::vec3f fragPos = (viewport.inverse() * (v1 * bary.x + v2 * bary.y + v3 * bary.z)).xyz;
    if (!smesh->normal_map)
        norm = kmath::normalize(n1 * bary.x + n2 * bary.y + n3 * bary.z);
    else {
        TGAcolor nm_ref = smesh->normal_map->get(tex_u * smesh->normal_map->getWidth(), (1 - tex_v) * smesh->normal_map->getHeight());
        for (int i = 0; i < 3; ++i) norm.v[i] = (float)nm_ref.raw[i] / 255.f * 2.f - 1.f;
        kmath::vec4f _norm = kmath::vec4f(norm, 0.f);
        _norm = kmath::normalize(model.inverse().transpose() * _norm);
        norm = _norm.xyz;
    }
    kmath::vec3f diff, spec, ambi;
    float shadow = 0.f;
    if (smesh->diffuse) {
        TGAcolor ref = smesh->diffuse->get(tex_u * smesh->diffuse->getWidth(), (1 - tex_v) * smesh->diffuse->getHeight());
        for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
    }
    else diff = prod(smesh->Kd, kmath::vec3f(255, 255, 255));
    kmath::vec3f lightDir = kmath::normalize(lightPos - fragPos) * lightIntensity;
    kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
    spec = prod(prod(lightColor, smesh->Ks), diff) * pow(max(0, norm * halfDir), gloss);
    diff = prod(diff * (max(norm * lightDir, 0.f) * 0.5 + 0.5), lightColor);
    ambi = prod(ambientColor, smesh->Ka);
    float x = round(doInterpolate(bary, lv1.x, lv2.x, lv3.x));
    float y = round(doInterpolate(bary, lv1.y, lv2.y, lv3.y));
    float sz = doInterpolate(bary, lv1.z, lv2.z, lv3.z);

    // 3x3 PCF
    int shadowCnt = 0, accessibleCnt = 9, p[] = { 0, 0, 1, 0, -1, 0, 0, 1, 0, -1, 1, 1, -1, 1, 1, -1, -1, -1 };
    for (int i = 0; i < 9; ++i) {
        int px = x + p[2 * i], py = y + p[2 * i + 1];
        if (px < 0 || py < 0 || px >= WINDOW_WIDTH || py >= WINDOW_HEIGHT) accessibleCnt--;
        else if (shadowbuffer[(int)(px * WINDOW_HEIGHT + py)] < sz + magic_num) shadowCnt++;
    }
    if (accessibleCnt == 0) shadow = 0.3;
    else shadow = (1.0 * shadowCnt / accessibleCnt);

    // simple shadow
    //if (x < 0 || y < 0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) shadow = .3;
    //else shadow = (shadowbuffer[(int)(x * WINDOW_HEIGHT + y)] < sz + magic_num) * .7 + .3;

    color = (diff + spec) * (shadow * 0.7 + 0.3) + ambi;
    cut_to_0_255(color);
    return true;
}