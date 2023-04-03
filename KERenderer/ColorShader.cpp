#include "ColorShader.h"

ColorShader::ColorShader(Mesh* m, kmath::vec3f c) {
    mesh = m;
    _color = c;
}
void ColorShader::vert(int face, int nface) {
    kmath::vec4f vec(mesh->vert[mesh->face[face][nface].x], 1.);
    vec = proj * view * model * vec;
    if (nface == 0) v1 = vec;
    else if (nface == 1) v2 = vec;
    else if (nface == 2) v3 = vec;
}
bool ColorShader::frag(kmath::vec3f& bary, kmath::vec3f& color, int nface, int i, int j) {
    color = _color;
    cut_to_0_255(color);
    return true;
}