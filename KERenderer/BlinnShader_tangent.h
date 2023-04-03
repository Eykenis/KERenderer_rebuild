#pragma once
#include "Shader.h"

extern kmath::mat4f model, view, proj, viewport;
extern kmath::vec3f lightPos;
extern kmath::vec3f lightColor;
extern kmath::vec3f cameraFront;

class BlinnShader_tangent :
    public Shader
{
private:
    kmath::vec3f Ks, Kd, Ka;
    kmath::vec3f t_lightDir[3], t_cameraFront[3];
    kmath::mat4f TBN;
public:
    float gloss;
    BlinnShader_tangent(Mesh* m, float _gloss = 100.);
    void vert(int face, int nface);
    bool frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i = 0, int j = 0);
};

