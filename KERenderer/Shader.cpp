#include "Shader.h"

float Shader::max_elevation_angle(float* zbuffer, kmath::vec2f p, kmath::vec2f dir) {
    float maxangle = 0;
    for (float t = 0.; t < 1000.; t += 10.) {
        kmath::vec2f cur = p + dir * t;
        if (cur.x >= WINDOW_WIDTH || cur.y >= WINDOW_HEIGHT || cur.x < 0 || cur.y < 0) return maxangle;

        float distance = kmath::module(p - cur);
        if (distance < 1.f) continue;
        float elevation = zbuffer[int(cur.x) * WINDOW_HEIGHT + int(cur.y)] - zbuffer[int(p.x) * WINDOW_HEIGHT + int(p.y)];
        maxangle = max(maxangle, atanf(elevation / distance));
    }
}

void Shader::sutherland_clip(kmath::vec4f clip_plane) {
    t_position.clear();
    t_nm_position.clear();
    t_uv_position.clear();
    t_worldzs.clear();
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
        }
        if (d1 < 0.f) {
            t_position.push_back(cur_vert);
            t_nm_position.push_back(nm_position[cur_index]);
            t_uv_position.push_back(uv_position[cur_index]);
        }
    }
    swap(t_position, position);
    swap(t_nm_position, nm_position);
    swap(t_uv_position, uv_position);
}

void Shader::work(float* buffer) {
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
        worldzs.clear();

        position.push_back(v1);
        position.push_back(v2);
        position.push_back(v3);

        nm_position.push_back(n1);
        nm_position.push_back(n2);
        nm_position.push_back(n3);

        uv_position.push_back(uv1);
        uv_position.push_back(uv2);
        uv_position.push_back(uv3);

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