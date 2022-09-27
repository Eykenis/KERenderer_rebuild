#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "draw2d.h"

int vertex_count = 4;
int tri_count = 2;

kmath::mat4f model, view, proj, viewport;
kmath::vec3f cameraPos, cameraUp, cameraFront;

int main() {
    clock_t start, end;
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EW_SHOWCONSOLE);
    Mesh mesh("./obj/african.obj");
    start = clock();

    // matrix transform
    proj = kmath::perspective(45.f, 1. * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(kmath::vec3f(0.f, 0.f, 3.f), kmath::vec3f(0.f, 0.f, -1.f), kmath::vec3f(0.f, 1.f, 0.f));
    model = kmath::model(kmath::vec3f(0.f, 0.f, 0.f), kmath::vec3f(1.f, 1.f, 1.f), kmath::vec3f(0.f, 0.f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    for (int i = 0; i < mesh.vert.size(); ++i) {
        kmath::vec4f vec(mesh.vert[i], 1.);
        vec = viewport * proj * view * model * vec;
        vec.x /= vec.w, vec.y /= vec.w, vec.z /= vec.w;
        vec.w /= vec.w;
        mesh.vert[i].x = vec.x;
        mesh.vert[i].y = vec.y;
        mesh.vert[i].z = vec.z;
    }

    // draw
    BeginBatchDraw();
    for (int i = 0; i < mesh.face.size(); ++i) {
        lineTriangle(mesh.vert[mesh.face[i][0].x].x,
                mesh.vert[mesh.face[i][0].x].y,
                mesh.vert[mesh.face[i][1].x].x,
                mesh.vert[mesh.face[i][1].x].y,
                mesh.vert[mesh.face[i][2].x].x,
                mesh.vert[mesh.face[i][2].x].y
            );
    }
    EndBatchDraw();

    end = clock();
    printf("%fs\n", 1. * (end - start) / CLOCKS_PER_SEC);
	system("pause");
	return 0;
}