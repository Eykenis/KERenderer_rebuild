#pragma once
#include "Shader.h"

extern float magic_num;
extern kmath::mat4f model, view, proj, viewport, lightSpaceMatrix;
extern float* shadowbuffer;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor, ambientColor;
extern float lightIntensity;

class BlinnShader_shadow :
    public Shader
{
public:
    float gloss;
    BlinnShader_shadow(Mesh* m, float _gloss = 100.) : Shader(m), gloss(_gloss) { }
    void vert(SubMesh* smesh, int face, int nface);
    bool frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

