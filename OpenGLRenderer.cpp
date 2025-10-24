// OpenGLRenderer.cpp
#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer() : hdc(nullptr), hrc(nullptr), colorR(0.0f), colorG(0.0f), colorB(0.0f) {}

OpenGLRenderer::~OpenGLRenderer()
{
    Cleanup();
}

bool OpenGLRenderer::Initialize(HWND hwnd)
{
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!pixelFormat)
        return false;

    if (!SetPixelFormat(hdc, pixelFormat, &pfd))
        return false;

    hrc = wglCreateContext(hdc);
    if (!hrc)
        return false;

    if (!wglMakeCurrent(hdc, hrc))
        return false;

    glViewport(0, 0, 800, 600);

    return true;
}

void OpenGLRenderer::Cleanup() {
    if (hrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hrc);
        hrc = nullptr;
    }
}

void OpenGLRenderer::SetClearColor(float r, float g, float b) {
    colorR = r;
    colorG = g;
    colorB = b;
}

void OpenGLRenderer::Render() {
    glClearColor(colorR, colorG, colorB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Solo dibujar fondo, sin figura inicial
    // La figura se dibujará desde las ventanas específicas
}

void OpenGLRenderer::SwapBuffers() {
    ::SwapBuffers(hdc);
}