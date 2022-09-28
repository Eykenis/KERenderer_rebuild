#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "PhongShader.h"
#include "draw2d.h"

kmath::mat4f model, view, proj, viewport;
kmath::vec3f cameraPos, cameraUp, cameraFront;
kmath::vec3f lightDir(0.f, 0.3f, 1.f); // trans
kmath::vec3f lightColor(255, 255, 255);

float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];

int main() {
    clock_t start, end;
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EW_SHOWCONSOLE);
    Mesh mesh("./obj/Aiz.obj");
    start = clock();

    lightDir = kmath::normalize(lightDir);
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
        for (int j = 0; j < WINDOW_HEIGHT; ++j) {
            zbuffer[i][j] = -1e10;
        }
    }

    // matrix transform
    proj = kmath::perspective(45.f, 1. * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(kmath::vec3f(0.f, 0.f, 3.f), kmath::vec3f(0.f, 0.f, -1.f), kmath::vec3f(0.f, 1.f, 0.f));
    model = kmath::model(kmath::vec3f(0.f, 30.f, 0.f), kmath::vec3f(5.f, 5.f, 5.f), kmath::vec3f(0.f, -1.4f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    PhongShader myshader(&mesh);

    myshader.vert();

    BeginBatchDraw();
    myshader.frag();
    EndBatchDraw();

    end = clock();
    printf("%fs\n", 1. * (end - start) / CLOCKS_PER_SEC);
	system("pause");
	return 0;
}