// FigureViewerWindow.cpp
#include "FigureViewerWindow.h"
#include <iostream>

FigureViewerWindow::FigureViewerWindow(const WindowConfig& config, std::shared_ptr<Figure> figureToView)
    : Window(config), figure(figureToView), hasPivot(false)
{
}

bool FigureViewerWindow::Create()
{
    if (!Window::Create())
        return false;

    // Configurar título de la ventana con el nombre de la figura
    if (figure)
    {
        std::string title = "Figure Viewer - " + figure->GetName() + " | Click para pivote | S/SHIFT+R/T para transformaciones";
        std::wstring wTitle(title.begin(), title.end());
        SetTitle(wTitle.c_str());
    }
    else
    {
        SetTitle(L"Figure Viewer - No Figure | Click para pivote");
    }

    return true;
}

LRESULT FigureViewerWindow::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(GetWindowHandle(), &ps);

        auto *renderer = GetRenderer();
        if (renderer)
        {
            // Asegurar que el contexto OpenGL esté activo
            wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

            renderer->Render();
            DrawSingleFigure();
            if (hasPivot) {
                DrawPivotPoint();
            }
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

        InvalidateRect(GetWindowHandle(), nullptr, FALSE);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        HandleClick(x, y);
        return 0;
    }

    case WM_KEYDOWN:
    {
        // Manejar atajos de teclado para transformaciones
        HandleKeyboard(wParam);
        return 0;
    }
    }

    return Window::HandleMessage(hwnd, msg, wParam, lParam);
}

void FigureViewerWindow::DrawSingleFigure()
{
    auto *renderer = GetRenderer();
    if (!renderer || !figure)
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    const auto &points = figure->GetPoints();
    if (points.size() < 2)
        return;

    // Obtener color de la figura
    Color figureColor = figure->GetColor();
    std::wcout << L"FigureViewer: Rendering figure with color (" << figureColor.r << L", " << figureColor.g << L", " << figureColor.b << L")" << std::endl;

    // Configuración del área de dibujo (usar toda la ventana)
    const float areaLeft = -0.8f;
    const float areaRight = 0.8f;
    const float areaTop = 0.8f;
    const float areaBottom = -0.8f;

    // Calcular el centro del área de dibujo
    float centerX = (areaLeft + areaRight) / 2.0f;
    float centerY = (areaTop + areaBottom) / 2.0f;

    // Calcular límites de la figura
    float minX = (std::numeric_limits<float>::max)();
    float maxX = (std::numeric_limits<float>::lowest)();
    float minY = (std::numeric_limits<float>::max)();
    float maxY = (std::numeric_limits<float>::lowest)();

    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);
        minX = (std::min)(minX, glX);
        maxX = (std::max)(maxX, glX);
        minY = (std::min)(minY, glY);
        maxY = (std::max)(maxY, glY);
    }

    float figureWidth = maxX - minX;
    float figureHeight = maxY - minY;

    // Calcular escala para que quepa en el área (con margen del 10%)
    float availableWidth = areaRight - areaLeft;
    float availableHeight = areaTop - areaBottom;

    float scaleX = (availableWidth * 0.9f) / figureWidth;
    float scaleY = (availableHeight * 0.9f) / figureHeight;
    float scale = (std::min)(scaleX, scaleY);

    // Si la figura es muy pequeña, usar escala 1:1 con límite superior
    if (scale > 1.0f)
        scale = 1.0f;

    // Establecer color de la figura
    glColor3f(figureColor.r, figureColor.g, figureColor.b);
    glLineWidth(3.0f);

    // Dibujar línea de la figura
    glBegin(GL_LINE_STRIP);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);

        // Aplicar escala y centrar en el área
        glX = centerX + (glX - (minX + maxX) / 2.0f) * scale;
        glY = centerY + (glY - (minY + maxY) / 2.0f) * scale;

        glVertex2f(glX, glY);
    }
    glEnd();

    // Dibujar puntos de la figura
    glColor3f(figureColor.r, figureColor.g, figureColor.b);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);

        // Aplicar escala y centrar en el área
        glX = centerX + (glX - (minX + maxX) / 2.0f) * scale;
        glY = centerY + (glY - (minY + maxY) / 2.0f) * scale;

        glVertex2f(glX, glY);
    }
    glEnd();
}

void FigureViewerWindow::HandleClick(int x, int y)
{
    // Convertir coordenadas de pantalla a OpenGL
    pivotPoint = ScreenToOpenGL(x, y);
    hasPivot = true;

    std::wcout << L"Pivot point set at: (" << pivotPoint.x << L", " << pivotPoint.y << L")" << std::endl;
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

HomogenVector FigureViewerWindow::ScreenToOpenGL(int screenX, int screenY)
{
    RECT rect;
    GetClientRect(GetWindowHandle(), &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Convertir coordenadas de pantalla a OpenGL (-1 a 1)
    float glX = (2.0f * screenX / width) - 1.0f;
    float glY = 1.0f - (2.0f * screenY / height);

    return HomogenVector::FromOpenGL(glX, glY);
}

void FigureViewerWindow::HandleKeyboard(WPARAM wParam)
{
    bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

    switch (wParam)
    {
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(), WM_CLOSE, 0, 0);
        break;

    // Manejar todas las combinaciones de teclas de flecha
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    {
        bool isLeft = (wParam == VK_LEFT);
        bool isRight = (wParam == VK_RIGHT);
        bool isUp = (wParam == VK_UP);
        bool isDown = (wParam == VK_DOWN);

        // Verificar combinaciones con teclas modificadoras
        if (GetKeyState('S') & 0x8000) {
            // S + Arrow (Scalar)
            if (isLeft || isRight) {
                scalar_x(isRight); // true para right, false para left
            } else if (isUp || isDown) {
                scalar_y(isUp); // true para up, false para down
            }
        }
        else if (GetKeyState('R') & 0x8000) {
            // R + Arrow (Rotate)
            if (isLeft || isRight) {
                if (isLeft) rotar_left();
                else rotar_right();
            }
        }
        else if (GetKeyState('T') & 0x8000) {
            // T + Arrow (Translate)
            if (isLeft || isRight) {
                trasladar_x(isRight); // true para right, false para left
            } else if (isUp || isDown) {
                trasladar_y(isUp); // true para up, false para down
            }
        }
        break;
    }
    }
}

void FigureViewerWindow::DrawPivotPoint()
{
    auto *renderer = GetRenderer();
    if (!renderer || !hasPivot)
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    // Dibujar punto pivote en rojo
    glColor3f(1.0f, 0.0f, 0.0f); // Rojo
    glPointSize(8.0f);

    glBegin(GL_POINTS);
    float glX, glY;
    pivotPoint.ToOpenGL(glX, glY);
    glVertex2f(glX, glY);
    glEnd();

    // Dibujar un pequeño cuadrado alrededor del pivote
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    float size = 0.02f;
    glBegin(GL_LINE_LOOP);
    glVertex2f(glX - size, glY - size);
    glVertex2f(glX + size, glY - size);
    glVertex2f(glX + size, glY + size);
    glVertex2f(glX - size, glY + size);
    glEnd();
}

// Funciones de transformación
void FigureViewerWindow::scalar_x(bool increase)
{
    PrintFigurePoints("scalar_x");
    std::string direction = increase ? "increase" : "decrease";
    std::wcout << L"Evento 'scalar_x " << direction.c_str() << L"' detectado" << std::endl;
}

void FigureViewerWindow::scalar_y(bool increase)
{
    PrintFigurePoints("scalar_y");
    std::string direction = increase ? "increase" : "decrease";
    std::wcout << L"Evento 'scalar_y " << direction.c_str() << L"' detectado" << std::endl;
}

void FigureViewerWindow::rotar_left()
{
    PrintFigurePoints("rotar_left");
    std::wcout << L"Evento 'rotar_left' detectado" << std::endl;
}

void FigureViewerWindow::rotar_right()
{
    PrintFigurePoints("rotar_right");
    std::wcout << L"Evento 'rotar_right' detectado" << std::endl;
}

void FigureViewerWindow::trasladar_x(bool right)
{
    PrintFigurePoints("trasladar_x");
    std::string direction = right ? "right" : "left";
    std::wcout << L"Evento 'trasladar_x " << direction.c_str() << L"' detectado" << std::endl;
}

void FigureViewerWindow::trasladar_y(bool up)
{
    PrintFigurePoints("trasladar_y");
    std::string direction = up ? "up" : "down";
    std::wcout << L"Evento 'trasladar_y " << direction.c_str() << L"' detectado" << std::endl;
}

void FigureViewerWindow::PrintFigurePoints(const std::string& eventName)
{
    if (!figure) return;

    std::wcout << L"=== " << eventName.c_str() << L" ===" << std::endl;
    const auto &points = figure->GetPoints();

    for (size_t i = 0; i < points.size(); ++i)
    {
        float glX, glY;
        points[i].ToOpenGL(glX, glY);
        std::wcout << L"Point " << i << L": (" << glX << L", " << glY << L")" << std::endl;
    }

    if (hasPivot)
    {
        float pivotX, pivotY;
        pivotPoint.ToOpenGL(pivotX, pivotY);
        std::wcout << L"Pivot: (" << pivotX << L", " << pivotY << L")" << std::endl;
    }

    std::wcout << L"================" << std::endl;
}
