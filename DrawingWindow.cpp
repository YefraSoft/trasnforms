// DrawingWindow.cpp
#include "DrawingWindow.h"
#include <iostream>

DrawingWindow::DrawingWindow(const WindowConfig& config)
    : Window(config), figureComplete(false), isDrawing(false)
{
    saveButton = std::make_unique<Button>(10, 10, 120, 30, L"Guardar Figura");
    instructionLabel = std::make_unique<Label>(150, 10, 300, 30, L"Dibuja tu figura");
}

bool DrawingWindow::Create()
{
    if (!Window::Create())
        return false;
    
    // Crear elementos UI
    saveButton->Create(GetWindowHandle());
    instructionLabel->Create(GetWindowHandle());
    
    // Configurar callback del botón
    saveButton->SetOnClick([this]() { OnSaveButtonClick(); });
    
    // Mostrar controles y forzar redibujado
    instructionLabel->Show();
    saveButton->Hide(); // Ocultar botón inicialmente
    UpdateWindow(GetWindowHandle());
    
    return true;
}

LRESULT DrawingWindow::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        OnMouseClick(x, y);
        return 0;
    }
    
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(GetWindowHandle(), &ps);
        
        GetRenderer()->Render();
        DrawLines();
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

void DrawingWindow::OnMouseClick(int x, int y)
{
    if (figureComplete)
        return;
    
    Point2D glPoint = ScreenToOpenGL(x, y);
    points.push_back(glPoint);
    
    std::wcout << L"Point added: (" << glPoint.x << L", " << glPoint.y << L")" << std::endl;
    
    CheckFigureComplete();
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

Point2D DrawingWindow::ScreenToOpenGL(int screenX, int screenY)
{
    RECT rect;
    GetClientRect(GetWindowHandle(), &rect);
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // Convertir coordenadas de pantalla a OpenGL (-1 a 1)
    float glX = (2.0f * screenX / width) - 1.0f;
    float glY = 1.0f - (2.0f * screenY / height);
    
    return Point2D(glX, glY);
}

void DrawingWindow::CheckFigureComplete()
{
    if (points.size() < 3)
        return;
    
    // Verificar si el último punto está cerca del primero (figura cerrada)
    const Point2D& first = points[0];
    const Point2D& last = points.back();
    
    float distance = sqrt((first.x - last.x) * (first.x - last.x) + 
                         (first.y - last.y) * (first.y - last.y));
    
    if (distance < 0.1f) // Tolerancia para cerrar la figura
    {
        figureComplete = true;
        saveButton->Show();
        instructionLabel->SetText(L"Figura completada! Guarda o dibuja otra.");
        std::wcout << L"Figure completed with " << points.size() << L" points" << std::endl;
    }
}

void DrawingWindow::DrawLines()
{
    if (points.size() < 2)
        return;
    
    glColor3f(1.0f, 1.0f, 0.0f); // Amarillo para las líneas
    glLineWidth(2.0f);
    
    glBegin(GL_LINE_STRIP);
    for (const auto& point : points)
    {
        glVertex2f(point.x, point.y);
    }
    glEnd();
    
    // Dibujar puntos
    glColor3f(1.0f, 0.0f, 0.0f); // Rojo para los puntos
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& point : points)
    {
        glVertex2f(point.x, point.y);
    }
    glEnd();
}

void DrawingWindow::OnSaveButtonClick()
{
    std::wcout << L"Figure saved with " << points.size() << L" points:" << std::endl;
    for (size_t i = 0; i < points.size(); ++i)
    {
        std::wcout << L"Point " << i << L": (" << points[i].x << L", " << points[i].y << L")" << std::endl;
    }
    
    // Aquí podrías implementar la lógica para retornar los puntos
    // Por ahora solo los mostramos en consola
}

void DrawingWindow::ClearDrawing()
{
    points.clear();
    figureComplete = false;
    saveButton->Hide();
    instructionLabel->SetText(L"Dibuja tu figura");
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}
