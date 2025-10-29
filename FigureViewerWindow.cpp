// FigureViewerWindow.cpp
#include "FigureViewerWindow.h"
#include <iostream>
#include <cmath>
#include <algorithm> // Para std::min y std::max
#include <limits>    // Para std::numeric_limits

FigureViewerWindow::FigureViewerWindow(const WindowConfig &config, const std::vector<std::shared_ptr<Figure>> &figuresToView)
    : Window(config), figures(figuresToView), currentFigureIndex(0), hasPivot(false), sPressed(false), tPressed(false), rPressed(false), gPressed(false)
{
    // Configurar botones de navegación carrusel
    leftButton = std::make_unique<Button>(10, 10, 50, 30, L"<-");
    rightButton = std::make_unique<Button>(70, 10, 50, 30, L"->");
}

bool FigureViewerWindow::Create()
{
    if (!Window::Create())
        return false;

    // Crear botones de navegación
    leftButton->Create(GetWindowHandle());
    rightButton->Create(GetWindowHandle());

    // Configurar callbacks de los botones
    leftButton->SetOnClick([this]()
                           { OnLeftButtonClick(); });
    rightButton->SetOnClick([this]()
                            { OnRightButtonClick(); });

    // Configurar título de la ventana con información de navegación
    UpdateButtonVisibility();

    return true;
}

void FigureViewerWindow::UpdateButtonVisibility()
{
    if (figures.size() <= 1)
    {
        leftButton->Hide();
        rightButton->Hide();
    }
    else
    {
        leftButton->Show();
        rightButton->Show();
    }
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
            if (hasPivot)
            {
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
        UpdateKeyState(wParam, true);
        HandleKeyboard(wParam);
        return 0;
    }

    case WM_KEYUP:
    {
        UpdateKeyState(wParam, false);
        return 0;
    }
    }

    return Window::HandleMessage(hwnd, msg, wParam, lParam);
}

void FigureViewerWindow::DrawSingleFigure()
{
    auto *renderer = GetRenderer();
    if (!renderer || figures.empty() || currentFigureIndex >= figures.size())
        return;

    auto figure = figures[currentFigureIndex];
    if (!figure)
        return;

    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    const auto &points = figure->GetPoints();
    if (points.size() < 2)
        return;

    Color figureColor = figure->GetColor();

    // Dibujar SIN recentrar
    glColor3f(figureColor.r, figureColor.g, figureColor.b);
    glLineWidth(3.0f);

    glBegin(GL_LINE_STRIP);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);
        glVertex2f(glX, glY); // Coordenadas directas
    }
    glEnd();

    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);
        glVertex2f(glX, glY);
    }
    glEnd();
}

void FigureViewerWindow::HandleClick(int x, int y)
{
    // Convertir coordenadas de pantalla a OpenGL
    pivotPoint = ScreenToOpenGL(x, y);
    hasPivot = true;
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

HomogenVector FigureViewerWindow::ScreenToOpenGL(int screenX, int screenY)
{
    RECT rect;
    GetClientRect(GetWindowHandle(), &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    float glX = (2.0f * screenX / width) - 1.0f;
    float glY = 1.0f - (2.0f * screenY / height);

    return HomogenVector::FromOpenGL(glX, glY);
}

void FigureViewerWindow::HandleKeyboard(WPARAM wParam)
{
    auto isDown = [](int vk) -> bool
    {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    };

    bool sDown = isDown('S');
    bool tDown = isDown('T');
    bool rDown = isDown('R');

    switch (wParam)
    {
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(), WM_CLOSE, 0, 0);
        break;

    case VK_RIGHT:
        if (tDown)
        {
            traslate(TRANSLATE_STEP, 0);
            break;
        }
        else if (sDown)
        {
            scale(SCALE_FACTOR, 1);
        }
        else if (rDown)
        {
            ROTATION_STEP += 0.2;
            if (ROTATION_STEP >= 360)
            {
                ROTATION_STEP -= 360;
            }
            rotate(ROTATION_STEP);
        }
        else
            NavigateToNextFigure();
        break;

    case VK_LEFT:
        if (sDown)
        {
            scale(1.0f / SCALE_FACTOR, 1);
        }
        else if (rDown)
        {
            ROTATION_STEP += 0.2f;
            if (ROTATION_STEP >= 360)
            {
                ROTATION_STEP -= 360;
            }
            rotate(-ROTATION_STEP);
        }
        else if (tDown)
        {
            traslate(-TRANSLATE_STEP, 0); // CORREGIDO: quitar el + 0.1f
        }
        else
            NavigateToPreviousFigure();
        break;

    case VK_UP:
        if (sDown)
        {
            scale(1, SCALE_FACTOR); // CORREGIDO: quitar el + 0.1f
        }
        else if (tDown)
        {
            traslate(0, TRANSLATE_STEP); // CORREGIDO: quitar el + 0.1f
        }
        break;

    case VK_DOWN:
        if (sDown)
        {
            scale(1, 1.0f / SCALE_FACTOR); // CORREGIDO
        }
        else if (tDown)
        {
            traslate(0, -TRANSLATE_STEP); // CORREGIDO: quitar el + 0.1f
        }
        break;
    }
}

void FigureViewerWindow::DrawPivotPoint()
{
    auto *renderer = GetRenderer();
    if (!renderer || !hasPivot)
        return;
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(8.0f);

    glBegin(GL_POINTS);
    float glX, glY;
    pivotPoint.ToOpenGL(glX, glY);
    glVertex2f(glX, glY);
    glEnd();

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

void FigureViewerWindow::NavigateToPreviousFigure()
{
    if (figures.size() <= 1)
        return;

    if (currentFigureIndex == 0)
        currentFigureIndex = figures.size() - 1;
    else
        currentFigureIndex--;
    hasPivot = false;
    ClearKeyState();
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

void FigureViewerWindow::NavigateToNextFigure()
{
    if (figures.size() <= 1)
        return;

    if (currentFigureIndex >= figures.size() - 1)
        currentFigureIndex = 0;
    else
        currentFigureIndex++;

    hasPivot = false;
    ClearKeyState();
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

void FigureViewerWindow::OnLeftButtonClick()
{
    NavigateToPreviousFigure();
}

void FigureViewerWindow::OnRightButtonClick()
{
    NavigateToNextFigure();
}

void FigureViewerWindow::UpdateKeyState(WPARAM wParam, bool pressed)
{
    switch (wParam)
    {
    case 'S':
        sPressed = pressed;
        break;
    case 'T':
        tPressed = pressed;
        break;
    case 'R':
        rPressed = pressed;
        break;
    }
}

void FigureViewerWindow::rotate(float degree)
{
    float angle = degree * PI / 180.0;
    float rotateMatrix[3][3] = {
        {std::cos(angle), -std::sin(angle), 0},
        {std::sin(angle), std::cos(angle), 0},
        {0, 0, 1}};
    if (figures.empty() || currentFigureIndex >= figures.size())
        return;

    auto figure = figures[currentFigureIndex];
    if (!figure)
        return;
    auto &points = figure->GetPoints();

    if (hasPivot)
    {
        float pivotX, pivotY;
        pivotPoint.ToOpenGL(pivotX, pivotY);

        for (auto &p : points)
        {
            p.x -= pivotX;
            p.y -= pivotY;
            p = matrix_prod(rotateMatrix, p);
            p.x += pivotX;
            p.y += pivotY;
        }
    }
    else
    {
        for (auto &p : points)
        {
            p = matrix_prod(rotateMatrix, p);
        }
    }
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
    UpdateWindow(GetWindowHandle());
}

void FigureViewerWindow::traslate(float tx, float ty)
{
    float traslateMatrix[3][3] = {
        {1, 0, tx},
        {0, 1, ty},
        {0, 0, 1}};

    if (figures.empty() || currentFigureIndex >= figures.size())
        return;

    auto figure = figures[currentFigureIndex];
    if (!figure)
        return;
    auto &points = figure->GetPoints();

    for (auto &p : points)
    {
        p = matrix_prod(traslateMatrix, p);
    }
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
    UpdateWindow(GetWindowHandle());
}

void FigureViewerWindow::scale(float sx, float sy)
{
    float scale_matrix[3][3] = {
        {sx, 0, 0},
        {0, sy, 0},
        {0, 0, 1}};

    if (figures.empty() || currentFigureIndex >= figures.size())
        return;

    auto figure = figures[currentFigureIndex];
    if (!figure)
        return;
    auto &points = figure->GetPoints();

    if (hasPivot)
    {
        float pivotX, pivotY;
        pivotPoint.ToOpenGL(pivotX, pivotY);

        for (auto &p : points)
        {
            p.x -= pivotX;
            p.y -= pivotY;
            p = matrix_prod(scale_matrix, p);
            p.x += pivotX;
            p.y += pivotY;
        }
    }
    else
    {
        // Escalar desde el origen si no hay pivote
        for (auto &p : points)
        {
            p = matrix_prod(scale_matrix, p);
        }
    }
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
    UpdateWindow(GetWindowHandle());
}

void FigureViewerWindow::ClearKeyState()
{
    sPressed = false;
    tPressed = false;
    rPressed = false;
    gPressed = false;
}

HomogenVector FigureViewerWindow::matrix_prod(float ma[3][3], HomogenVector mb)
{
    float x = mb.x;
    float y = mb.y;

    float nx = ma[0][0] * x + ma[0][1] * y + ma[0][2] * 1;
    float ny = ma[1][0] * x + ma[1][1] * y + ma[1][2] * 1;

    return HomogenVector(nx, ny, 1.0f);
    ;
}