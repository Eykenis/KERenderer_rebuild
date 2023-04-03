#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern float* zbuffer, shadowbuffer;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor;

class ShadowShader :
    public Shader
{
public:
    ShadowShader(Mesh* m);
private:
    void vert(int face, int nface);
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

