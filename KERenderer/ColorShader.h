#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor;
extern kmath::vec3f cameraFront;

class ColorShader :
    public Shader
{
private:
    kmath::vec3f _color;
public:
    ColorShader(Mesh* m, kmath::vec3f c);
    void vert(SubMesh* smesh, int face, int nface);
    bool frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

