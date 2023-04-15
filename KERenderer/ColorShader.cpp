#include "ColorShader.h"

void ColorShader::vert(SubMesh* smesh, int face, int nface) {
    kmath::vec4f vec(mesh->vert[smesh->face[face][nface].x], 1.);
    vec = proj * view * model * vec;
    if (nface == 0) v1 = vec;
    else if (nface == 1) v2 = vec;
    else if (nface == 2) v3 = vec;
}
bool ColorShader::frag(SubMesh* smesh, kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    color = _color;
    cut_to_0_255(color);
    return true;
}