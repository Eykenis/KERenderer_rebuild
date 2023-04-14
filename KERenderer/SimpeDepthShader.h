#pragma once
#include "Shader.h"

extern kmath::mat4f lightSpaceMatrix, viewport, model;
extern float* zbuffer, *shadowbuffer;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor;

class SimpleDepthShader :
    public Shader
{
public:
    SimpleDepthShader(Mesh* m);
private:
    void vert(SubMesh* smesh, int face, int nface);
    bool frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

