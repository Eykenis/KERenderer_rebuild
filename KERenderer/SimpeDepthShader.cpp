#include "SimpeDepthShader.h"

SimpleDepthShader::SimpleDepthShader(Mesh* m) {
    mesh = m;
}
void SimpleDepthShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    vec = (lightSpaceMatrix * model) * vec;
    if (nface == 0) v1 = vec;
    else if (nface == 1) v2 = vec;
    else if (nface == 2) v3 = vec;
}
bool SimpleDepthShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    color = kmath::vec3f(255., 255., 255) * (doInterpolate(bary, v1.z, v2.z, v3.z) / 2.f);
    cut_to_0_255(color);
    return false;
}