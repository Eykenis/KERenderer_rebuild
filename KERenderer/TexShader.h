#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor;
extern kmath::vec3f cameraPos;

class TexShader :
    public Shader
{
public:
    TexShader(Mesh* m);
    void vert(int face, int nface);
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};
