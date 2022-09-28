#pragma once
#include "Shader.h"
extern kmath::mat4f model, view, proj, viewport;
extern float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];
extern kmath::vec3f lightDir;
extern kmath::vec3f lightColor;

class PhongShader :
    public Shader
{
private:
    Mesh* mesh;
public:
    PhongShader(Mesh* m) {
        mesh = m;
    }
    void vert() {
        for (int i = 0; i < mesh->vert.size(); ++i) {
            kmath::vec4f vec(mesh->vert[i], 1.);
            vec = viewport * proj * view * model * vec;
            vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
            vec.w /= vec.w;
            mesh->vert[i].x = vec.x;
            mesh->vert[i].y = vec.y;
            mesh->vert[i].z = vec.z;
        }

        for (int i = 0; i < mesh->normal.size(); ++i) {
            mesh->normal[i] = normalize(mesh->normal[i]);
        }
    }
    void frag() {
        for (int i = 0; i < mesh->face.size(); ++i) {
            kmath::vec3f c[3];
            for (int j = 0; j < 3; ++j) {
                c[j] = lightColor * ((lightDir * mesh->normal[mesh->face[i][j].z]));
                c[j].x = max(0, c[j].x); c[j].x = min(c[j].x, 255);
                c[j].y = max(0, c[j].y); c[j].y = min(c[j].y, 255);
                c[j].z = max(0, c[j].z); c[j].z = min(c[j].z, 255);
            }
            drawTriangle(mesh->vert[mesh->face[i][0].x],
                mesh->vert[mesh->face[i][1].x],
                mesh->vert[mesh->face[i][2].x],
                mesh->normal[mesh->face[i][0].z],
                mesh->normal[mesh->face[i][1].z],
                mesh->normal[mesh->face[i][2].z]
            );
        }
    }
};

