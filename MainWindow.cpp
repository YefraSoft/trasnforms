// MainWindow.cpp
#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(const WindowConfig& config)
    : Window(config)
{
    titleLabel = std::make_unique<Label>(50, 50, 300, 40, L"Transformaciones Geométricas");
    drawButton = std::make_unique<Button>(150, 150, 150, 40, L"Abrir Dibujo");
}

bool MainWindow::Create()
{
    if (!Window::Create())
        return false;
    
    // Crear elementos UI
    titleLabel->Create(GetWindowHandle());
    drawButton->Create(GetWindowHandle());
    
    // Configurar callback del botón
    drawButton->SetOnClick([this]() { OnDrawButtonClick(); });
    
    // Forzar redibujado de los controles
    titleLabel->Show();
    drawButton->Show();
    UpdateWindow(GetWindowHandle());
    
    return true;
}

LRESULT MainWindow::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            OnDrawButtonClick();
        }
        return 0;
    }
    
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(GetWindowHandle(), &ps);
        
        GetRenderer()->Render();
        GetRenderer()->SwapBuffers();
        
        EndPaint(GetWindowHandle(), &ps);
        return 0;
    }
    
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        glViewport(0, 0, width, height);
        InvalidateRect(GetWindowHandle(), nullptr, FALSE);
        return 0;
    }
    }
    
    return Window::HandleMessage(hwnd, msg, wParam, lParam);
}

void MainWindow::OnDrawButtonClick()
{
    std::wcout << L"Opening drawing window..." << std::endl;
    
    // Crear ventana de dibujo
    WindowConfig drawConfig(L"Ventana de Dibujo", 800, 600, 200, 100);
    drawingWindow = std::make_unique<DrawingWindow>(drawConfig);
    
    if (drawingWindow->Create())
    {
        drawingWindow->Show();
        std::wcout << L"Drawing window created successfully" << std::endl;
    }
    else
    {
        std::wcout << L"Failed to create drawing window" << std::endl;
    }
}
