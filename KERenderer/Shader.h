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
class Shader
{
protected:
    Mesh* mesh;
    kmath::vec4f v1, v2, v3;
    kmath::vec2f uv1, uv2, uv3;
    kmath::vec3f n1, n2, n3;
    kmath::vec3f t, b, n;
    kmath::vec3f worldz;
    std::vector < kmath::vec4f> position;
    std::vector < kmath::vec3f> nm_position;
    std::vector < kmath::vec2f> uv_position;

    std::vector < kmath::vec4f> t_position;
    std::vector < kmath::vec3f> t_nm_position;
    std::vector < kmath::vec2f> t_uv_position;

    int i, j;
private:
    float max_elevation_angle(float* zbuffer, kmath::vec2f p, kmath::vec2f dir);
    void sutherland_clip(kmath::vec4f clip_plane);
public:
	virtual void vert(int face, int nface) = 0;
	virtual bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0) = 0;

    void work(float* buffer, int antialiasing);
};