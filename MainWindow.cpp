// MainWindow.cpp
#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(const WindowConfig &config)
    : Window(config), figureCounter(0)
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
    drawButton->SetOnClick([this]()
                           { OnDrawButtonClick(); });

    // Configurar callback global para figuras
    FigureManager::SetGlobalCallback([this](std::shared_ptr<Figure> figure)
                                     { OnFigureComplete(figure); });

    // Forzar redibujado de los controles
    titleLabel->Show();
    drawButton->Show();
    UpdateWindow(GetWindowHandle());

    // Debug: verificar tamaño inicial
    RECT rect;
    GetClientRect(GetWindowHandle(), &rect);
    std::wcout << L"DEBUG: MainWindow initial size: " << (rect.right - rect.left) << L"x" << (rect.bottom - rect.top) << std::endl;

    // Configurar viewport correcto para MainWindow
    glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);
    std::wcout << L"DEBUG: MainWindow viewport set to " << (rect.right - rect.left) << L"x" << (rect.bottom - rect.top) << std::endl;

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

        auto *renderer = GetRenderer();
        if (renderer)
        {
            // Asegurar que el contexto OpenGL esté activo
            wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

            std::wcout << L"DEBUG: WM_PAINT called in MainWindow" << std::endl;
            renderer->Render();
            DrawAllFigures();
            renderer->SwapBuffers();
        }

        EndPaint(GetWindowHandle(), &ps);
        return 0;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        if (width > 0 && height > 0) // Evitar divisiones por cero
        {
            auto *renderer = GetRenderer();
            if (renderer)
            {
                // Asegurar que el contexto OpenGL esté activo
                wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());
                glViewport(0, 0, width, height);
            }
        }

        std::wcout << L"DEBUG: MainWindow resized to " << width << L"x" << height << std::endl;
        std::wcout << L"DEBUG: MainWindow viewport updated to " << width << L"x" << height << std::endl;
        InvalidateRect(GetWindowHandle(), nullptr, FALSE);
        return 0;
    }
    }

    return Window::HandleMessage(hwnd, msg, wParam, lParam);
}

bool MainWindow::HasActiveWindows() const
{
    // Verificar si la ventana principal está activa
    if (Window::IsActive())
        return true;

    // Verificar si alguna ventana de dibujo está activa
    for (const auto &drawingWindow : drawingWindows)
    {
        if (drawingWindow && drawingWindow->IsActive())
            return true;
    }

    return false;
}

void MainWindow::OnDrawButtonClick()
{
    std::wcout << L"Opening drawing window..." << std::endl;

    // Verificar si ya hay ventanas de dibujo activas
    int activeDrawingWindows = 0;
    for (const auto &drawingWindow : drawingWindows)
    {
        if (drawingWindow && drawingWindow->IsActive())
            activeDrawingWindows++;
    }

    if (activeDrawingWindows > 0)
    {
        std::wcout << L"Warning: There are already " << activeDrawingWindows << L" active drawing windows. Please wait." << std::endl;
        return;
    }

    // Crear ventana de dibujo con nombre único
    figureCounter++;
    std::string figureName = "Figure_" + std::to_string(figureCounter);

    WindowConfig drawConfig(L"Ventana de Dibujo", 800, 600, 200, 100);
    auto drawingWindow = std::make_unique<DrawingWindow>(drawConfig, figureName);

    if (drawingWindow->Create())
    {
        drawingWindow->Show();
        // Almacenar la ventana para que no se destruya
        drawingWindows.push_back(std::move(drawingWindow));
        std::wcout << L"Drawing window created successfully for " << figureName.c_str() << std::endl;
    }
    else
    {
        std::wcout << L"Failed to create drawing window" << std::endl;
    }
}

void MainWindow::OnFigureComplete(std::shared_ptr<Figure> figure)
{
    std::wcout << L"Figure completed: " << figure->GetName().c_str() << L" with " << figure->GetPointCount() << L" points" << std::endl;

    // Agregar figura a la lista
    figures.push_back(figure);

    // Remover la ventana de dibujo completada
    if (!drawingWindows.empty())
    {
        drawingWindows.erase(drawingWindows.begin());
        std::wcout << L"Drawing window removed after figure completion" << std::endl;
    }

    // Debug: verificar estado de figuras
    std::wcout << L"DEBUG: Total figures in MainWindow: " << figures.size() << std::endl;

    // CORRECCIÓN: Forzar repintado inmediato completo de la ventana principal
    RedrawWindow(GetWindowHandle(), nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

void MainWindow::DrawAllFigures()
{
    auto *renderer = GetRenderer();
    if (!renderer || figures.empty())
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    std::wcout << L"DEBUG: DrawAllFigures called with " << figures.size() << L" figures" << std::endl;

    // Dibujar todas las figuras centradas
    for (size_t i = 0; i < figures.size(); ++i)
    {
        const auto &figure = figures[i];
        const auto &points = figure->GetPoints();

        if (points.size() < 2)
            continue;

        // Color diferente para cada figura
        float colorR = (i % 3 == 0) ? 1.0f : 0.0f;
        float colorG = (i % 3 == 1) ? 1.0f : 0.0f;
        float colorB = (i % 3 == 2) ? 1.0f : 0.0f;

        std::wcout << L"DEBUG: Using color (" << colorR << L", " << colorG << L", " << colorB << L") for figure " << i << std::endl;

        glColor3f(colorR, colorG, colorB);
        glLineWidth(2.0f);

        glBegin(GL_LINE_STRIP);
        for (const auto &point : points)
        {
            float glX, glY;
            point.ToOpenGL(glX, glY);

            // Escalar las coordenadas para que quepan en el viewport de MainWindow
            // Las figuras originales están en el rango [-1, 1] de DrawingWindow (800x600)
            // Pero MainWindow es más pequeña (500x300), así que las escalamos para que sean visibles
            float scale = 0.8f; // Escalar para que queden dentro del viewport
            glX *= scale;
            glY *= scale;

            std::wcout << L"DEBUG: Drawing point (" << glX << L", " << glY << L") for figure " << i << std::endl;
            glVertex2f(glX, glY);
        }
        glEnd();

        // Dibujar puntos
        glColor3f(colorR, colorG, colorB);
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (const auto &point : points)
        {
            float glX, glY;
            point.ToOpenGL(glX, glY);

            // Escalar las coordenadas para que quepan en el viewport de MainWindow
            // Las figuras originales están en el rango [-1, 1] de DrawingWindow (800x600)
            // Pero MainWindow es más pequeña (500x300), así que las escalamos para que sean visibles
            float scale = 0.8f; // Escalar para que queden dentro del viewport
            glX *= scale;
            glY *= scale;

            glVertex2f(glX, glY);
        }
        glEnd();
    }
}
