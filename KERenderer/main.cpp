#include "api.h"
#include "macro.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "kmath.h"
#include "Mesh.h"
#include "Shaders.h"
#include "afterprocess.h"

float xPos, yPos, lastX, lastY; // mouse position
kmath::mat4f model, view, proj, viewport, lightSpaceMatrix;
kmath::vec3f cameraPos(0.f, 0.f, 3.f), cameraUp(0.f, 1.f, 0.f), cameraFront(0.f, 0.f, -1.f), cameraRight(1.f, 0.f, 0.f);
kmath::vec3f lightPos(1.414f, 1.f, 1.414f); // trans
kmath::vec3f lightColor(255, 255, 255);
float        lightIntensity = 1, magic_num = 0.00034;

bool stencil_read = 0, stencil_write = 0;
float yaw, pitch;
float* zbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

float* shadowbuffer = new float[WINDOW_WIDTH * WINDOW_HEIGHT];

unsigned char* stencilbuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT];
unsigned char* framebuffer = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
window_t* window = NULL;

void clearframebuffer(unsigned char*);
void lighttexcutting();
void mouse_process();

int main() {
    clock_t start, end;
    printf("Enter:\n 0 - Blinn Shading\t1 - Blinn Shading with Shadow\t2 - Blinn Shading with Tangent-Space Normal Map\n");
    printf("3 - Ramp Shading\t4 - Only UV Texture\t5 - Color Shading\n");
    int rendermode = 0;
    cin >> rendermode;

    window_init(WINDOW_WIDTH, WINDOW_HEIGHT, L"KERenderer");
    Mesh mesh("./obj/Aiz.obj", "./tex/Aiz_diffuse.tga");
    Mesh mesh2("./obj/African.obj", "./tex/African_diffuse.tga", "./tex/African_normal_tangent.tga");
    start = clock();
    
    cameraFront = kmath::normalize(cameraFront);

    float y = 0.f, height = 0.f;

    // matrix transform
    //proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    //view = kmath::lookat(cameraPos, cameraFront, cameraUp);
    //model = kmath::model(kmath::vec3f(0.f, 0.f, 0.f), kmath::vec3f(1.f, 1.f, 1.f), kmath::vec3f(0.f, -1.2f, 0.f));
    viewport = kmath::viewport(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    Shader* shader1 = nullptr;
    Shader* shader2 = nullptr;

    switch (rendermode) {
    case 0:
        shader1 = new BlinnShader(&mesh);
        break;
    case 1:
        shader1 = new BlinnShader_shadow(&mesh);
        shader2 = new SimpleDepthShader(&mesh);
        break;
    case 2:
        shader1 = new BlinnShader_tangent(&mesh2);
        break;
    case 3:
        shader1 = new RampShader(&mesh, "./tex/Ramp.tga");
        break;
    case 4:
        shader1 = new TexShader(&mesh);
        break;
    case 5:
        shader1 = new ColorShader(&mesh, kmath::vec3f(150.f, 150.f, 150.f));
        break;
    default:
        break;
    }

    TexShader myshader(&mesh);
    SimpleDepthShader depthshader(&mesh);

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
        proj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 10.0f);
        view = kmath::lookat(cameraPos, kmath::normalize(cameraFront), cameraUp);
        model = kmath::model(kmath::vec3f(0.f, -30.f + y, 0.f), kmath::vec3f(1.f, 1.f, 1.f), kmath::vec3f(0.f, -0.79f, 0.f));
        
        lighttexcutting();

        clearframebuffer(framebuffer);

        if (shader2) shader2->work(shadowbuffer);
        if (shader1) shader1->work(zbuffer);

        //filter(framebuffer);

        window_draw(framebuffer);
        mouse_process();
        msg_dispatch();
        if (GetAsyncKeyState(0x41)) cameraPos = cameraPos - cameraRight * 0.1;
        if (GetAsyncKeyState(0x44)) cameraPos = cameraPos + cameraRight * 0.1;
        if (GetAsyncKeyState(0x57)) cameraPos = cameraPos + cameraFront * 0.1;
        if (GetAsyncKeyState(0x53)) cameraPos = cameraPos - cameraFront * 0.1;
        if (GetAsyncKeyState(VK_F1)) y += 2;
        if (GetAsyncKeyState(VK_F2)) y -= 2;
        if (GetAsyncKeyState(VK_F3)) lightIntensity += 0.05;
        if (GetAsyncKeyState(VK_F4)) lightIntensity -= 0.05;
        if (GetAsyncKeyState(VK_F5)) lightPos = (kmath::model(kmath::vec3f(0.f, 2.f, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(lightPos, 1.f)).xyz;
        if (GetAsyncKeyState(VK_F6)) lightPos = (kmath::model(kmath::vec3f(0.f, -2.f, 0.f), kmath::vec3f(1.0f, 1.0f, 1.0f), kmath::vec3f(0.f, 0.f, 0.f)) * kmath::vec4f(lightPos, 1.f)).xyz;
        if (GetAsyncKeyState(VK_F7)) magic_num += 0.0001;
        if (GetAsyncKeyState(VK_F8)) magic_num -= 0.0001;
        if (GetAsyncKeyState(VK_F9)) cameraFront = (kmath::rotate(0.f, 1.f, 0.f) * kmath::vec4f(cameraFront, 0.f)).xyz;
        if (GetAsyncKeyState(VK_F10)) cameraFront = (kmath::rotate(0.f, -1.f, 0.f) * kmath::vec4f(cameraFront, 0.f)).xyz;
        if (GetAsyncKeyState(VK_SPACE)) cameraPos.y += 0.05;
        if (GetAsyncKeyState(VK_LSHIFT)) cameraPos.y -= 0.05;
        if (GetAsyncKeyState(VK_ESCAPE)) break;
        if (GetAsyncKeyState(VK_UP)) height += 0.05;
        if (GetAsyncKeyState(VK_DOWN)) height -= 0.05;
        //Sleep(16);
    }
    //system("pause");
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
    kmath::mat4f lightView = kmath::lookat(lightPos * 2, -(lightPos * 2), kmath::vec3f(0.f, 1.f, 0.f));
    kmath::mat4f lightProj = kmath::perspective(45.f, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    //kmath::mat4f lightProj = kmath::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f);
    lightSpaceMatrix = lightProj * lightView;
}

void mouse_process() {
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