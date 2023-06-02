#include "Shader.h"

float Shader::max_elevation_angle(float* zbuffer, kmath::vec2f p, kmath::vec2f dir) {
    float maxangle = 0;
    for (float t = 0.; t < 1000.; t += 100.) {
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
    t_lposition.clear();
    t_worldPoses.clear();

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
            t_worldPoses.push_back(worldPoses[cur_index] * t + worldPoses[pre_index] * (1.0f - t));
        }
        if (d1 < 0.f) {
            t_position.push_back(cur_vert);
            t_nm_position.push_back(nm_position[cur_index]);
            t_uv_position.push_back(uv_position[cur_index]);
            t_lposition.push_back(lposition[cur_index]);
            t_worldPoses.push_back(worldPoses[cur_index]);
        }
    }
    swap(t_position, position);
    swap(t_nm_position, nm_position);
    swap(t_uv_position, uv_position);
    swap(t_lposition, lposition);
    swap(t_worldPoses, worldPoses);
}

void Shader::work(float* buffer) {
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
        for (int j = 0; j < WINDOW_HEIGHT; ++j) {
            buffer[i * WINDOW_HEIGHT + j] = -1e10;
        }
    } // clear buffer
    // for every submesh in mesh
    int meshsize = mesh->submesh.size();

    bool* vis = new bool[meshsize];
    std::fill(vis, vis + meshsize, false);

    for (int _nMesh = 0; _nMesh < 2 * meshsize; ++_nMesh) {
        int nMesh = _nMesh % meshsize;
        if (vis[nMesh] && mesh->submesh[nMesh].d == 1.0) continue;
        vis[nMesh] = true;
        // Process every triangle face
        SubMesh* smesh = &mesh->submesh[nMesh];
        for (int k = 0; k < smesh->face.size(); ++k) {
            for (int i = 0; i < 3; ++i) {
                uv[i] = mesh->tex_coord[smesh->face[k][i].y];
            }
            kmath::getTBN(t, b, mesh->vert[smesh->face[k][0].x], mesh->vert[smesh->face[k][1].x], mesh->vert[smesh->face[k][2].x], uv[0], uv[1], uv[2]);
            vert(smesh, k, 0);
            vert(smesh, k, 1);
            vert(smesh, k, 2);

            position.clear();
            nm_position.clear();
            uv_position.clear();
            lposition.clear();
            worldPoses.clear();

            for (int i = 0; i < 3; ++i) {
                position.push_back(pos[i]);
                nm_position.push_back(worldNormal[i]);
                uv_position.push_back(uv[i]);
                lposition.push_back(lightSpacePos[i]);
                worldPoses.push_back(worldPos[i]);
            }

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

                Z = (lposition[i].w == 0 ? 1 : 1 / lposition[i].w);
                lposition[i] = lposition[i] * Z;
                lposition[i].w = 1.0f;

                lposition[i] = viewport * lposition[i];
            }

            for (int nv = 0; nv < num_vertex - 2; ++nv) {
                int idx[3] = { 0, nv + 1, nv + 2 };
                for (int i = 0; i < 3; ++i) {
                    pos[i] = position[idx[i]];
                    uv[i] = uv_position[idx[i]];
                    lightSpacePos[i] = lposition[idx[i]];
                    worldNormal[i] = nm_position[idx[i]];
                    worldPos[i] = worldPoses[idx[i]];
                }

                float xl = min(pos[0].x, min(pos[1].x, pos[2].x)), xr = max(pos[0].x, max(pos[1].x, pos[2].x));
                float yd = min(pos[0].y, min(pos[1].y, pos[2].y)), yu = max(pos[0].y, max(pos[1].y, pos[2].y));
                if ((xr <= 0 || xl >= WINDOW_WIDTH) || (yu <= 0 || yd >= WINDOW_HEIGHT)) continue;
                kmath::vec4f v21 = pos[1] - pos[0];
                kmath::vec4f v32 = pos[2] - pos[0];
                kmath::vec3f crs = cross(v21.xyz, v32.xyz);
                // backface culling
                if (crs.z <= 0) continue;
                float z;
                for (int i = xl; i <= xr; ++i) {
                    if (i <= 0) continue;
                    if (i >= WINDOW_WIDTH) break;
                    for (int j = yd; j <= yu; ++j) {
                        if (j <= 0) continue;
                        if (j >= WINDOW_HEIGHT) break;
                        kmath::vec3f interpolate;

                        interpolate = barycentric(kmath::vec2f(i, j), kmath::vec2f(pos[0].x, pos[0].y), kmath::vec2f(pos[1].x, pos[1].y), kmath::vec2f(pos[2].x, pos[2].y));
                        
                        bool inTri = inTriangle(interpolate);

                        // perspective interpolation correction
                        //for (int i = 0; i < 3; ++i) interpolate[i] /= worldPoses[i].z - cameraPos.z;
                        //float fz = 1. / (interpolate[0] + interpolate[1] + interpolate[2]);
                        //for (int i = 0; i < 3; ++i) interpolate[i] *= fz;

                        
                        kmath::vec3f color;

                        if (inTri > 0 && (z = doInterpolate(interpolate, pos[0].z, pos[1].z, pos[2].z)) > buffer[i * WINDOW_HEIGHT + j]) {
                            if (smesh->d == 1.0) {
                                buffer[WINDOW_HEIGHT * i + j] = z;
                            }
                            int idx = (i * WINDOW_HEIGHT + j);

                            if (!stencil_read || (stencil_read && stencilbuffer[idx] != 1)) {
                                if (frag(smesh, interpolate, color, k, i, j)) {
                                    drawpixel(framebuffer, i, j, color, smesh->d);
                                }
                                if (stencil_write) stencilbuffer[idx] = 1;
                            }
                            if (render_to_texture) {
                                float u = uv[0].x * interpolate.x + uv[1].x * interpolate.y + uv[2].x * interpolate.z;
                                float v = uv[0].y * interpolate.x + uv[1].y * interpolate.y + uv[2].y * interpolate.z;
                                target[_nMesh]->setColor(round(u * smesh->diffuse->getWidth()), round(v * smesh->diffuse->getHeight()), TGAcolor(color.b, color.g, color.r));
                            }
                        }
                    }
                }
            }
        }
        //delete smesh;
    }
    // SSAO
    //for (int x = 0; x <WINDOW_WIDTH; x++) {
    //    for (int y = 0; y < WINDOW_HEIGHT; y++) {
    //        if (zbuffer[x * WINDOW_HEIGHT + y] < -1e5) continue;
    //        float total = 0;
    //        int cnt = 0;
    //        for (float a = 0; a < 6.28; a += 6.28 / 4) {
    //            for (float b = 0; b < 6.28; b += 6.28 / 4) {
    //                for (float c = 0.33; c <= 1; c += 0.33) {
    //                    float radius = 12;
    //                    kmath::vec3f vec(x + radius * cos(b) * cos(a) * c, y + radius * cos(b) * sin(a) * c, zbuffer[x * WINDOW_HEIGHT + y] + radius * sin(b) * c);
    //                    if (vec.x >= WINDOW_WIDTH || vec.y >= WINDOW_HEIGHT || vec.x < 0 || vec.y < 0) continue;
    //                    if (vec.z <= zbuffer[(int)vec.x * WINDOW_HEIGHT + (int)vec.y]) total++;
    //                    cnt++;
    //                }
    //            }
    //        }
    //        total = min(1, max(0, (cnt - total) / (cnt / 2)));
    //        total = log2(total + 1);
    //        intensity(framebuffer, x, y, kmath::vec3f(total, total, total));
    //    }
    //}

    if (render_to_texture) {
        for (int i = 0; i < target.size(); ++i) {
            char buffer[128];
            sprintf_s(buffer, "./tex/diffuse%02d.tga", i + 1);
            //target[i]->flip_vertically();
            target[i]->write_TGA(buffer);
        }
    }
}