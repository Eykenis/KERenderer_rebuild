#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport, lightSpaceMatrix;
extern float* shadowbuffer;
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

class BlinnShader_shadow :
    public Shader
{
private:
    kmath::vec3f Ks, Kd, Ka;
    kmath::vec3f lv1, lv2, lv3;
public:
    float gloss;
    BlinnShader_shadow(Mesh* m, float _gloss = 100.) {
        mesh = m;
        gloss = _gloss;
        Ks = kmath::vec3f(0.2f, 0.2f, 0.2f);
        Ka = kmath::vec3f(0.1f, 0.1f, 0.1f);
        Kd = kmath::vec3f(1.f, 1.f, 1.f);
    }
    void vert(int face, int nface) {
        kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
        kmath::vec4f norm(mesh->normal[mesh->face[face][nface].z], 0.);
        vec = viewport * proj * view * model * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        norm = kmath::normalize(model.inverse().transpose() * norm);
        if (nface == 0) v1 = vec.xyz, n1 = norm.xyz, uv1 = mesh->tex_coord[mesh->face[face][0].y];
        else if (nface == 1) v2 = vec.xyz, n2 = norm.xyz, uv2 = mesh->tex_coord[mesh->face[face][1].y];
        else if (nface == 2) v3 = vec.xyz, n3 = norm.xyz, uv3 = mesh->tex_coord[mesh->face[face][2].y];

        vec = kmath::vec4f(mesh->vert[mesh->face[face][nface].x], 1.f);
        vec = (viewport * lightSpaceMatrix * model) * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        if (nface == 0) lv1 = vec.xyz; else if (nface == 1) lv2 = vec.xyz; else if (nface == 2) lv3 = vec.xyz;
    }
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) {
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
        kmath::vec3f diff, spec, ambi;
        float shadow = 0.f;
        if (mesh->diffuse) {
            TGAcolor ref = mesh->diffuse->get(tex_u * mesh->diffuse->getWidth(), (1 - tex_v) * mesh->diffuse->getHeight());
            for (int i = 0; i < 3; ++i) diff.v[i] = ref.raw[i];
            kmath::vec3f halfDir = kmath::normalize(lightDir - cameraFront);
            spec = (prod(lightColor, Ks)) * pow(max(0, norm * halfDir), gloss);
            diff = (prod(Kd, diff)) * (max(norm * lightDir, 0.f));
            ambi = prod(lightColor, Ka);
            kmath::vec3f shadow_bary = (view * lightSpaceMatrix * model * (viewport * proj * view * model).inverse() * kmath::vec4f(bary, 0.f)).xyz;
            float x = round(doInterpolate(bary, lv1.x, lv2.x, lv3.x));
            float y = round(doInterpolate(bary, lv1.y, lv2.y, lv3.y));
            float sz = doInterpolate(bary, lv1.z, lv2.z, lv3.z);
            if (x < 0 || y < 0) shadow = .3;
            shadow = .3 + .7 * (shadowbuffer[(int)(x * WINDOW_HEIGHT + y)] < sz + 0.004334);
            color = (diff + spec) * shadow + ambi;
        }
        else color = lightColor * (lightDir * norm);
        cut_to_0_255(color);
        return true;
    }
};

