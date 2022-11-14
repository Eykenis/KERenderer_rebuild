#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "Shaders.h"

kmath::mat4f model, view, proj, viewport, lightSpaceMatrix;
kmath::vec3f cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.f, 0.f), cameraFront(0.f, 0.f, -1.f);
kmath::vec3f lightDir(0.717f, 0.f, 0.717f); // trans
kmath::vec3f lightColor(255, 255, 255);

bool stencil_read = 0, stencil_write = 0;
float* zbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

float* shadowbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

unsigned char* stencilbuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT];
unsigned char* _shadowbuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
unsigned char* framebuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
window_t* window = NULL;

void clearframebuffer(unsigned char*);
void lighttexcutting();

int main() {
    clock_t start, end;
    window_init(WINDOW_WIDTH, WINDOW_HEIGHT, L"KERenderer");
    Mesh mesh("./obj/African.obj", "./tex/African_diffuse.tga", "./tex/African_normal.tga");
    Mesh mesh2("./obj/Aiz.obj", "./tex/Aiz_diffuse.tga");
    start = clock();
    
    cameraFront = kmath::normalize(cameraFront);

    float y = 0.f;

    // matrix transform
    proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    view = kmath::lookat(cameraPos, cameraFront, cameraUp);
    model = kmath::model(kmath::vec3f(0.f, 0.f, 0.f), kmath::vec3f(1.4f, 1.4f, 1.4f), kmath::vec3f(0.f, -1.2f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    SimpleDepthShader depthshader(&mesh);
    BlinnShader_shadow binnshader(&mesh, 100.f);

    lighttexcutting();

    int fps = 0;
    //30.f per sec
    while (!window->is_close) {
        fps++;
        end = clock();
        if (end - start > CLOCKS_PER_SEC) {
            printf("FPS: %d\n", fps);
            start = clock();
            fps = 0;
        }
        proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        view = kmath::lookat(cameraPos, cameraFront, cameraUp);
        model = kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(1.f, 1.f, 1.f), kmath::vec3f(0.f, 0.f, 0.f));

        clearframebuffer(framebuffer);

        //cameraFront = (kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(0.0f, 0.0f, -0.1f, 1.0f)).xyz;
        depthshader.work(shadowbuffer);
        binnshader.work(zbuffer);
        window_draw(framebuffer);
        msg_dispatch();
        if (GetAsyncKeyState(VK_LEFT)) cameraPos.x += 0.05;
        if (GetAsyncKeyState(VK_RIGHT)) cameraPos.x -= 0.05;
        if (GetAsyncKeyState(VK_UP)) cameraPos.z -= 0.05;
        if (GetAsyncKeyState(VK_DOWN)) cameraPos.z += 0.05;
        if (GetAsyncKeyState(VK_F1)) y += 2;
        if (GetAsyncKeyState(VK_F2)) y -= 2;
        Sleep(33);
    }
    window_destroy();
	return 0;
}

void clearframebuffer(unsigned char* framebuffer) {
    for (int i = 0; i < WINDOW_HEIGHT; ++i) {
        for (int j = 0; j < WINDOW_WIDTH; ++j) {
            int index = (i * WINDOW_WIDTH + j) * 4;
            framebuffer[index + 2] = 97;
            framebuffer[index + 1] = 63;
            framebuffer[index] = 110;
        }
    }
}

void lighttexcutting() {
    kmath::mat4f lightView = kmath::lookat(lightDir * 2, -(lightDir * 2), kmath::vec3f(0.f, 1.f, 0.f));
    kmath::mat4f lightProj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    lightSpaceMatrix = lightProj * lightView;
}