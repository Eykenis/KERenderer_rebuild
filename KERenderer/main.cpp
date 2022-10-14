#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "PhongShader.h"

int vertex_count = 4;
int tri_count = 2;

kmath::mat4f model, view, proj, viewport;
kmath::vec3f cameraPos, cameraUp, cameraFront;
kmath::vec3f lightDir(0.f, 0.0f, 1.f); // trans
kmath::vec3f lightColor(255, 255, 255);

float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];

int main() {
    clock_t start, end;
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EW_SHOWCONSOLE);
    setbkcolor(0x4f6f6f);
    cleardevice();
    Mesh mesh("./obj/Aiz.obj", L"./tex/Aiz_diffuse.tga");
    start = clock();

    lightDir = kmath::normalize(lightDir);
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
        for (int j = 0; j < WINDOW_HEIGHT; ++j) {
            zbuffer[i][j] = -1e10;
        }
    }

    // matrix transform
    proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(kmath::vec3f(0.f, 0.f, 3.f), kmath::vec3f(0.f, 0.f, -1.f), kmath::vec3f(0.f, 1.f, 0.f));
    model = kmath::model(kmath::vec3f(0.f, -30.f, 0.f), kmath::vec3f(1.2f, 1.2f, 1.2f), kmath::vec3f(0.f, -0.8f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    PhongShader myshader(&mesh);

    int fps = 0;
    float y = 0.f;
    // 30.f per sec
    //while (1) {
    //    //clock_t st, ed;
    //    //st = clock();
    //    fps++;
    //    end = clock();
    //    if (end - start >= CLOCKS_PER_SEC) {
    //        printf("FPS %d\n", fps);
    //        start = clock();
    //        fps = 0;
    //    }
    //    model = kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(1.f, 1.f, 1.f), kmath::vec3f(0.f, 0.f, 0.f));
    //    if (GetAsyncKeyState(VK_LEFT)) y += 5.f;
    //    else if (GetAsyncKeyState(VK_RIGHT)) y -= 5.f;
    //    myshader.work();
    //    //ed = clock();
    //    //y += 1. * (ed - st) / CLOCKS_PER_SEC * 30.f;
    //    //Sleep(33);
    //}

    myshader.work();
	system("pause");
	return 0;
}