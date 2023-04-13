#include "BlinnShader_shadow.h"

BlinnShader_shadow::BlinnShader_shadow(Mesh* m, float _gloss) {
    mesh = m;
    gloss = _gloss;
    Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
    Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
    Kd = kmath::vec3f(1.f, 1.f, 1.f);
}

void BlinnShader_shadow::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    kmath::vec4f norm(mesh->normal[mesh->face[face][nface].z], 0.);
    //kmath::mat4f m = viewport * proj * view * model;
    vec = model * vec;
    worldz.v[nface] = vec.z;
    vec = proj * view * vec;
    norm = kmath::normalize(model.inverse().transpose() * norm);
    if (nface == 0) v1 = vec, n1 = norm.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
    else if (nface == 1) v2 = vec, n2 = norm.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
    else if (nface == 2) v3 = vec, n3 = norm.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];

    vec = kmath::vec4f(mesh->vert[mesh->face[face][nface].x], 1.f);
    vec = (lightSpaceMatrix * model) * vec;
    if (nface == 0) lv1 = vec; else if (nface == 1) lv2 = vec; else if (nface == 2) lv3 = vec;
}
bool BlinnShader_shadow::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    float tex_u = uv1.x * bary.x + uv2.x * bary.y + uv3.x * bary.z;
    float tex_v = uv1.y * bary.x + uv2.y * bary.y + uv3.y * bary.z;
    kmath::vec3f norm;
    kmath::vec3f fragPos = (viewport.inverse() * (v1 * bary.x + v2 * bary.y + v3 * bary.z)).xyz;
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
    float shadow = 0.f;
    if (mesh->diffuse) {
        TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
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

void BlinnShader_shadow::work(float* buffer) {
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
        for (int j = 0; j < WINDOW_HEIGHT; ++j) {
            buffer[i * WINDOW_HEIGHT + j] = -1e10;
        }
    } // clear buffer
    // Process every triangle face
    for (int k = 0; k < mesh->face.size(); ++k) {
        uv1 = mesh->tex_coord[mesh->face[k][0].y];
        uv2 = mesh->tex_coord[mesh->face[k][1].y];
        uv3 = mesh->tex_coord[mesh->face[k][2].y];
        kmath::getTBN(t, b, mesh->vert[mesh->face[k][0].x], mesh->vert[mesh->face[k][1].x], mesh->vert[mesh->face[k][2].x], uv1, uv2, uv3);
        vert(k, 0);
        vert(k, 1);
        vert(k, 2);

        position.clear();
        nm_position.clear();
        uv_position.clear();
        lposition.clear();

        position.push_back(v1);
        position.push_back(v2);
        position.push_back(v3);

        nm_position.push_back(n1);
        nm_position.push_back(n2);
        nm_position.push_back(n3);

        uv_position.push_back(uv1);
        uv_position.push_back(uv2);
        uv_position.push_back(uv3);

        lposition.push_back(lv1);
        lposition.push_back(lv2);
        lposition.push_back(lv3);

        // Homogeneous Clipping
        sutherland_clip(kmath::vec4f(0, 0, 1, 1));
        sutherland_clip(kmath::vec4f(0, 0, -1, 1));
        sutherland_clip(kmath::vec4f(1, 0, 0, 1));
        sutherland_clip(kmath::vec4f(-1, 0, 0, 1));
        sutherland_clip(kmath::vec4f(0, -1, 0, 1));
        sutherland_clip(kmath::vec4f(0, 1, 0, 1));

        int num_vertex = position.size();

        // Simple Clipping
        //bool flag = 0;
        //for (int i = 0; i < num_vertex; ++i) {
        //    if ((position[i].x <= position[i].w || position[i].x >= -position[i].w) || (position[i].y <= position[i].w || position[i].y >= -position[i].w)) {
        //        flag = 1;
        //        break;
        //    }
        //    if (position[i].w > 0) {
        //        flag = 1;
        //        break;
        //    }
        //}
        //if (flag) continue;

        // perspective division & **viewport transformation**
        for (int i = 0; i < num_vertex; ++i) {
            float Z = 1 / position[i].w;
            position[i] = position[i] * Z;
            position[i].w = 1.0f;

            position[i] = viewport * position[i];

            Z = 1 / lposition[i].w;
            lposition[i] = lposition[i] * Z;
            lposition[i].w = 1.0f;

            lposition[i] = viewport * lposition[i];
        }

        for (int nv = 0; nv < num_vertex - 2; ++nv) {
            int idx1 = 0, idx2 = nv + 1, idx3 = nv + 2;

            v1 = position[idx1];
            n1 = nm_position[idx1];
            uv1 = uv_position[idx1];
            v2 = position[idx2];
            n2 = nm_position[idx2];
            uv2 = uv_position[idx2];
            v3 = position[idx3];
            n3 = nm_position[idx3];
            uv3 = uv_position[idx3];
            lv1 = lposition[idx1];
            lv2 = lposition[idx2];
            lv3 = lposition[idx3];

            float xl = min(v1.x, min(v2.x, v3.x)), xr = max(v1.x, max(v2.x, v3.x));
            float yd = min(v1.y, min(v2.y, v3.y)), yu = max(v1.y, max(v2.y, v3.y));
            if ((xr <= 0 || xl >= WINDOW_WIDTH) || (yu <= 0 || yd >= WINDOW_HEIGHT)) continue;
            kmath::vec4f v21 = v2 - v1;
            kmath::vec4f v32 = v3 - v1;
            kmath::vec3f crs = cross(v21.xyz, v32.xyz);
            // backface culling
            if (crs.z <= 0) continue;
            float z;
            for (i = xl; i <= xr; ++i) {
                if (i <= 0) continue;
                if (i >= WINDOW_WIDTH) break;
                for (j = yd; j <= yu; ++j) {
                    if (j <= 0) continue;
                    if (j >= WINDOW_HEIGHT) break;
                    kmath::vec3f interpolate;
                    int in_count = 0;
                    bool inflag[4] = { 0, 0, 0, 0 };

                    interpolate = barycentric(kmath::vec2f(i, j), kmath::vec2f(v1.x, v1.y), kmath::vec2f(v2.x, v2.y), kmath::vec2f(v3.x, v3.y));

                    in_count = inTriangle(interpolate);

                    if (in_count > 0 && (z = doInterpolate(interpolate, v1.z, v2.z, v3.z)) > buffer[i * WINDOW_HEIGHT + j]) {
                        buffer[WINDOW_HEIGHT * i + j] = z;
                        int idx = (i * WINDOW_HEIGHT + j);
                        kmath::vec3f color;
                        if (!stencil_read || (stencil_read && stencilbuffer[idx] != 1)) {
                            if (frag(interpolate, color, k, i, j)) {
                                drawpixel(framebuffer, i, j, color);
                            }
                            if (stencil_write) stencilbuffer[idx] = 1;
                        }
                    }
                }
            }
        }
    }
    // SSAO
    //for (int x = 0; x <WINDOW_WIDTH; x++) {
    //    for (int y = 0; y < WINDOW_HEIGHT; y++) {
    //        if (zbuffer[x * WINDOW_HEIGHT + y] < -1e5) continue;
    //        float total = 0;
    //        for (float a = 0; a < 3.14 * 2 - 1e-4; a += 3.14 / 4) {
    //            total += 3.14 / 2 - max_elevation_angle(zbuffer, kmath::vec2f(x, y), kmath::vec2f(cos(a), sin(a)));
    //        }
    //        total /= (3.14 / 2) * 8;
    //        total = pow(total, 1000.f);
    //        intensity(framebuffer, x, y, kmath::vec3f(total, total, total));
    //    }
    //}
}