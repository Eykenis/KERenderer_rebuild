#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "PhongShader.h"
#include "BlinnShader.h"


kmath::mat4f model, view, proj, viewport;
kmath::vec3f cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.f, 0.f), cameraFront(0.f, 0.f, -1.f);
kmath::vec3f lightDir(0.f, 0.0f, 1.f); // trans
kmath::vec3f lightColor(255, 255, 255);

float zbuffer[WINDOW_WIDTH][WINDOW_HEIGHT];
unsigned char* framebuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
window_t* window = NULL;

void clearframebuffer(unsigned char*);

int main() {
    clock_t start, end;
    window_init(WINDOW_WIDTH, WINDOW_HEIGHT, L"KERenderer");
    Mesh mesh("./obj/Aiz.obj", "./tex/Aiz_diffuse.tga");
    start = clock();

    // matrix transform
    proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(cameraPos, cameraFront, cameraUp);
    model = kmath::model(kmath::vec3f(0.f, 0.f, 0.f), kmath::vec3f(3.2f, 3.2f, 3.2f), kmath::vec3f(0.f, -1.4f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    BlinnShader myshader(&mesh, 105.f);

    int fps = 0;
    float y = -30.f;
    //30.f per sec
    while (!window->is_close) {
        fps++;
        end = clock();
        if (end - start > CLOCKS_PER_SEC) {
            printf("FPS: %d\n", fps);
            start = clock();
            fps = 0;
        }
        clearframebuffer(framebuffer);
        myshader.work();
        window_draw(framebuffer);
        msg_dispatch();
        model = kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(3.2f, 3.2f, 3.2f), kmath::vec3f(0.f, -1.4f, 0.f));
        //lightDir = (model * kmath::vec4f(0.f, 0.f, 1.f, 0.f)).xyz;
        if (GetAsyncKeyState(VK_LEFT)) y++;
        else if (GetAsyncKeyState(VK_RIGHT)) y--;
        Sleep(33);
    }
    window_destroy();
	return 0;
}

void clearframebuffer(unsigned char* framebuffer) {
    for (int i = 0; i < WINDOW_HEIGHT; ++i) {
        for (int j = 0; j < WINDOW_WIDTH; ++j) {
            int index = (i * WINDOW_WIDTH + j) * 4;
            framebuffer[index + 2] = 214;
            framebuffer[index + 1] = 233;
            framebuffer[index] = 248;
        }
    }
}