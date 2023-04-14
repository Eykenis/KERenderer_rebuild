#include "BlinnShader_shadow.h"

BlinnShader_shadow::BlinnShader_shadow(Mesh* m, float _gloss) {
    mesh = m;
    gloss = _gloss;
    Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
    Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
    Kd = kmath::vec3f(1.f, 1.f, 1.f);
}

void BlinnShader_shadow::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[smesh->face[face][nface].z], 0.);
    //kmath::mat4f m = viewport * proj * view * model;
    vec = model * vec;
    worldz.v[nface] = vec.z;
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
        kmath::vec3f lightDir = kmath::normalize(lightPos - fragPos) * lightIntensity;
        kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
        spec = (prod(lightColor, Ks)) * pow(max(0, norm * halfDir), gloss);
        diff = (prod(Kd, diff)) * (max(norm * lightDir, 0.f) * 0.5 + 0.5);
        ambi = prod(lightColor, Ka);
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
        else shadow = (1.0 * shadowCnt / accessibleCnt) * 0.7 + 0.3;

        // simple shadow
        //if (x < 0 || y < 0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) shadow = .3;
        //else shadow = (shadowbuffer[(int)(x * WINDOW_HEIGHT + y)] < sz + magic_num) * .7 + .3;

        color = (diff + spec) * shadow + ambi;
    }
    else color = lightColor * (lightPos * norm);
    cut_to_0_255(color);
    return true;
}

void BlinnShader_shadow::sutherland_clip(kmath::vec4f clip_plane) {
    t_position.clear();
    t_nm_position.clear();
    t_uv_position.clear();
    t_lposition.clear();

    int sz = position.size();
    for (int i = 0; i < sz; ++i) {
        int cur_index = i, pre_index = (i - 1 + sz) % sz;
        kmath::vec4f cur_vert = position[cur_index], pre_vert = position[pre_index];
        float d1 = clip_plane.x * cur_vert.x + clip_plane.y * cur_vert.y + clip_plane.z * cur_vert.z + clip_plane.w * cur_vert.w;
        float d2 = clip_plane.x * pre_vert.x + clip_plane.y * pre_vert.y + clip_plane.z * pre_vert.z + clip_plane.w * pre_vert.w;
        if (d1 * d2 < 0.f) {
            // calculte interpolation
            float t = d2 / (d2 - d1);
            t_position.push_back(cur_vert * t + pre_vert * (1.0f - t));
            t_nm_position.push_back(nm_position[cur_index] * t + nm_position[pre_index] * (1.0f - t));
            t_uv_position.push_back(uv_position[cur_index] * t + uv_position[pre_index] * (1.0f - t));
            t_lposition.push_back(lposition[cur_index] * t + lposition[pre_index] * (1.0f - t));
        }
        if (d1 < 0.f) {
            t_position.push_back(cur_vert);
            t_nm_position.push_back(nm_position[cur_index]);
            t_uv_position.push_back(uv_position[cur_index]);
            t_lposition.push_back(lposition[cur_index]);
        }
    }
    swap(t_position, position);
    swap(t_nm_position, nm_position);
    swap(t_uv_position, uv_position);
    swap(t_lposition, lposition);
}