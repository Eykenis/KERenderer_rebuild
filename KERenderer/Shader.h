#pragma once
#include "api.h"
#include "Mesh.h"
#include "draw2d.h"

extern unsigned char* framebuffer;
extern float* zbuffer;
extern kmath::vec3f lightPos, cameraFront, cameraPos;
extern unsigned char* stencilbuffer;
extern bool stencil_write, stencil_read;
extern unsigned char* msaabuffer;
extern kmath::mat4f viewport;
extern kmath::mat4f model, view, proj, lightSpaceMatrix;
class Shader
{
protected:
    Mesh* mesh;
    kmath::vec3f t, b, n;
    kmath::vec4f pos[3];
    kmath::vec2f uv[3];
    kmath::vec3f worldNormal[3];
    kmath::vec4f worldPos[3];
    kmath::vec4f lightSpacePos[3];

    bool render_to_texture;
    std::vector<TGAimage*> target;

    std::vector < kmath::vec4f> position;
    std::vector < kmath::vec2f> uv_position;
    std::vector < kmath::vec3f> nm_position;
    std::vector < kmath::vec4f> worldPoses;

    std::vector < kmath::vec4f> t_position;
    std::vector < kmath::vec3f> t_nm_position;
    std::vector < kmath::vec2f> t_uv_position;
    std::vector < kmath::vec4f> t_worldPoses;

    std::vector < kmath::vec4f> t_lposition, lposition; // for shadow mapping


    float max_elevation_angle(float* zbuffer, kmath::vec2f p, kmath::vec2f dir);
    virtual void sutherland_clip(kmath::vec4f clip_plane);

public:
    Shader(Mesh* m, bool render = true) : mesh(m), render_to_texture(render) {
        if (!render) return;
        target.resize(m->submesh.size());
        for (int i = 0; i < target.size(); ++i) {
            target[i] = new TGAimage(m->submesh[i].diffuse->getWidth(), m->submesh[i].diffuse->getHeight(), m->submesh[i].diffuse->getBpp());
        }
    }

	virtual void vert(SubMesh* smesh, int face, int nface) = 0;
	virtual bool frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) = 0;

    virtual void work(float* buffer);
};