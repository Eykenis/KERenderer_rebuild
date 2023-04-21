#include "SimpeDepthShader.h"

SimpleDepthShader::SimpleDepthShader(Mesh* m) : Shader(m) { }

void SimpleDepthShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    vec = (lightSpaceMatrix * model) * vec;
    pos[nface] = vec;
}
bool SimpleDepthShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    color = kmath::vec3f(255., 255., 255) * (doInterpolate(bary, pos[0].z, pos[1].z, pos[2].z) / 2.f);
    cut_to_0_255(color);
    return false;
}