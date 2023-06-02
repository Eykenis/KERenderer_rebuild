#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor, ambientColor;
extern kmath::vec3f cameraFront;
extern kmath::vec3f cameraPos;
extern float lightIntensity;

class BlinnShader :
    public Shader
{
public:
    float gloss;
    BlinnShader(Mesh* m, float _gloss = 100., bool render = false) : Shader(m, render), gloss(_gloss) {

    }
    void vert(SubMesh* smesh, int face, int nface);
    bool frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

