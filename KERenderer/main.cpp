#pragma once
#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "draw2d.h"
#include "PhongShader.h"
#include "BlinnShader.h"

kmath::mat4f model, view, proj, viewport;
kmath::vec3f cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.f, 0.f), cameraFront(0.f, 0.f, -1.f);
kmath::vec3f lightDir(0.f, 0.0f, 1.f); // trans
kmath::vec3f lightColor(255, 255, 255);
float zbuffer[WINDOW_WIDTH + 5][WINDOW_HEIGHT + 5];
float ambient;

int main() {
    clock_t start, end;
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT, EW_SHOWCONSOLE);
    setbkcolor(0x4f6f6f);
    cleardevice();
    Mesh mesh("./obj/Aiz.obj", "./tex/Aiz_diffuse.tga");
    start = clock();

    lightDir = kmath::normalize(lightDir);

    // matrix transform
    proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(cameraPos, cameraFront, cameraUp);
    model = kmath::model(kmath::vec3f(0.f, 0.f, 0.f), kmath::vec3f(5.f, 5.f, 5.f), kmath::vec3f(0.f, -1.4f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    PhongShader myshader(&mesh);

    int fps = 0;
    float y = 0.f;
    while (1) {
        end = clock();
        if (end - start > CLOCKS_PER_SEC) {
            start = clock();
            printf("FPS: %d\n", fps);
            fps = 0;
        }
        fps++;
        myshader.work();
        if (GetAsyncKeyState(VK_LEFT)) {
            y += 5;
        }
        if (GetAsyncKeyState(VK_RIGHT)) {
            y -= 5;
        }
        model = kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(5.f, 5.f, 5.f), kmath::vec3f(0.f, -1.4f, 0.f));
    }

	system("pause");
	return 0;
}