#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "Shaders.h"
#include "afterprocess.h"

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

float xPos, yPos, lastX, lastY; // mouse position
kmath::mat4f model, view, proj, viewport, lightSpaceMatrix;
kmath::vec3f cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.f, 0.f), cameraFront(0.f, 0.f, -1.f), cameraRight(1.f, 0.f, 0.f);
kmath::vec3f lightPos(2.414f, 0.f, 2.414f); // trans
kmath::vec3f lightColor(255, 255, 255);
float        lightIntensity = 1, magic_num = 0.00234;

bool stencil_read = 0, stencil_write = 0;
float yaw, pitch;
float* zbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

float* shadowbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

unsigned char* msaabuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 16]; // 2x2 MSAA
unsigned char* stencilbuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT];
unsigned char* _shadowbuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
unsigned char* framebuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
window_t* window = NULL;

void clearframebuffer(unsigned char*);
void lighttexcutting();
void mouse_callback();

int main() {
    clock_t start, end;
    window_init(WINDOW_WIDTH, WINDOW_HEIGHT, L"KERenderer");
    //Mesh mesh("./obj/African.obj", "./tex/african_diffuse.tga", "./tex/african_normal_tangent.tga");
    Mesh mesh2("./obj/Aiz.obj", "./tex/Aiz_diffuse.tga");
    Mesh grass("./obj/back.obj", "./tex/back.tga");
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

    end = clock();
    printf("%fs\n", 1. * (end - start) / CLOCKS_PER_SEC);
	system("pause");
	return 0;
    viewport = kmath::viewport(0, 640, 0, 480);
    //lightPos = normalize(lightPos);

    //SimpleDepthShader depthshader(&mesh);
    //BlinnShader myshader(&mesh2, 100.f);
    BlinnShader myshader(&mesh2);

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
        lighttexcutting();
        proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.0f);
        view = kmath::lookat(cameraPos, kmath::normalize(cameraFront), cameraUp);
        model = kmath::model(kmath::vec3f(0.f, -30.f + y, 0.f), kmath::vec3f(1.6f, 1.6f, 1.6f), kmath::vec3f(0.f, -1.7f, 0.f));

        clearframebuffer(framebuffer);

        //cameraFront = (kmath::model(kmath::vec3f(0.f, y, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(0.0f, 0.0f, -0.1f, 1.0f)).xyz;
        //depthshader.work(shadowbuffer);
        myshader.work(zbuffer, 0);
        //filter(framebuffer);
        window_draw(framebuffer);
        mouse_callback();
        msg_dispatch();
        if (GetAsyncKeyState(0x41)) cameraPos = cameraPos - cameraRight * 0.05;
        if (GetAsyncKeyState(0x44)) cameraPos = cameraPos + cameraRight * 0.05;
        if (GetAsyncKeyState(0x57)) cameraPos = cameraPos + cameraFront * 0.05;
        if (GetAsyncKeyState(0x53)) cameraPos = cameraPos - cameraFront * 0.05;
        if (GetAsyncKeyState(VK_F1)) y += 2;
        if (GetAsyncKeyState(VK_F2)) y -= 2;
        if (GetAsyncKeyState(VK_F3)) lightIntensity += 0.05;
        if (GetAsyncKeyState(VK_F4)) lightIntensity -= 0.05;
        if (GetAsyncKeyState(VK_F5)) lightPos = (kmath::model(kmath::vec3f(0.f, 2.f, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(lightPos, 1.f)).xyz;
        if (GetAsyncKeyState(VK_F6)) lightPos = (kmath::model(kmath::vec3f(0.f, -2.f, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(lightPos, 1.f)).xyz;
        if (GetAsyncKeyState(VK_F7)) magic_num += 0.000001;
        if (GetAsyncKeyState(VK_F8)) magic_num -= 0.000001;
        if (GetAsyncKeyState(VK_F9)) cameraFront = (kmath::rotate(0.f, 1.f, 0.f) * kmath::vec4f(cameraFront, 0.f)).xyz;
        if (GetAsyncKeyState(VK_F10)) cameraFront = (kmath::rotate(0.f, -1.f, 0.f) * kmath::vec4f(cameraFront, 0.f)).xyz;
        if (GetAsyncKeyState(VK_SPACE)) cameraPos.y += 0.05;
        if (GetAsyncKeyState(VK_CONTROL)) cameraPos.y -= 0.05;
        if (GetAsyncKeyState(VK_ESCAPE)) break;
        Sleep(33);
    }
    system("pause");
    window_destroy();
	return 0;
}

void clearbuffer(unsigned char* buffer) {
    for (int i = 0; i < WINDOW_HEIGHT * 2; ++i) {
        for (int j = 0; j < WINDOW_WIDTH * 2; ++j) {
            int index = (i * WINDOW_WIDTH + j) * 4;
            framebuffer[index + 2] = 97;
            framebuffer[index + 1] = 63;
            framebuffer[index] = 110;
        }
    }
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
    kmath::mat4f lightView = kmath::lookat(lightPos, -lightPos, kmath::vec3f(0.f, 1.f, 0.f));
    kmath::mat4f lightProj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    lightSpaceMatrix = lightProj * lightView;
}

void mouse_callback() {
    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;
    //lastX = xPos, lastY = yPos;

    float sensitivity = 0.1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    cameraFront = (kmath::rotate(pitch, yaw, 0) * kmath::vec4f(0.f, 0.f, -1.f, 0.f)).xyz;
    cameraUp = (kmath::rotate(pitch, yaw, 0) * kmath::vec4f(0.f, 1.f, 0.f, 0.f)).xyz;
    cameraRight = kmath::cross(cameraUp, cameraFront);
}