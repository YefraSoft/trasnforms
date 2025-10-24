// Window.cpp
#include "Window.h"
#include <iostream>

const wchar_t *WINDOW_CLASS_NAME = L"OpenGLWindowClass";

Window::Window(const WindowConfig &cfg)
    : hwnd(nullptr), config(cfg), active(false)
{
    renderer = std::make_unique<OpenGLRenderer>();
}

Window::~Window()
{
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }
}

bool Window::Create()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    // Crear un nombre de clase único para cada ventana
    static int windowCounter = 0;
    windowCounter++;
    std::wstring uniqueClassName = L"OpenGLWindowClass" + std::to_wstring(windowCounter);
    
    // Registrar clase de ventana única
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = StaticWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);
    wc.lpszClassName = uniqueClassName.c_str();

    RegisterClassExW(&wc);

    // Debug: verificar el título antes de crear la ventana
    std::wcout << L"Creating window with title: '" << config.title << L"'" << std::endl;
    
    // Crear ventana usando CreateWindowW (más simple)
    hwnd = CreateWindowW(
        uniqueClassName.c_str(),
        config.title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        config.posX, config.posY,
        config.width, config.height,
        nullptr, nullptr, hInstance, this);

    if (!hwnd)
        return false;

    // Inicializar OpenGL
    if (!renderer->Initialize(hwnd))
    {
        return false;
    }

    renderer->SetClearColor(0.2f, 0.3f, 0.4f);
    active = true;

    return true;
}

void Window::Show()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

void Window::Hide()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_HIDE);
    }
}

void Window::SetTitle(const wchar_t *title)
{
    if (hwnd)
    {
        SetWindowTextW(hwnd, title);
    }
}

HWND Window::GetHandle() const
{
    return hwnd;
}

bool Window::IsActive() const
{
    return active;
}

void Window::SetRenderColor(float r, float g, float b)
{
    renderer->SetClearColor(r, g, b);
}

const WindowConfig& Window::GetConfig() const
{
    return config;
}

LRESULT CALLBACK Window::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window *window = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        window = reinterpret_cast<Window *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    }
    else
    {
        window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->HandleMessage(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        renderer->Render();
        renderer->SwapBuffers();
        ValidateRect(hwnd, nullptr);
        return 0;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        glViewport(0, 0, width, height);
        InvalidateRect(hwnd, nullptr, FALSE); // Redibujar después del cambio de tamaño
        return 0;
    }

    case WM_CLOSE:
        active = false;
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        active = false;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}