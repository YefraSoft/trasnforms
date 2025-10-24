// OpenGLRenderer.h - Single Responsibility Principle
#pragma once
#include <windows.h>
#include <gl/gl.h>

class OpenGLRenderer
{
private:
    HDC hdc;
    HGLRC hrc;
    float colorR, colorG, colorB;

public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    bool Initialize(HWND hwnd);
    void Cleanup();
    void SetClearColor(float r, float g, float b);
    void Render();
    void SwapBuffers();

    // Agregar método para acceder al contexto OpenGL
    HGLRC GetGLRC() const { return hrc; }
};
