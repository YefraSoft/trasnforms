// DrawingWindow.cpp
#include "DrawingWindow.h"
#include "FigureCallback.h"
#include <iostream>

DrawingWindow::DrawingWindow(const WindowConfig &config, const std::string &name)
    : Window(config), figureComplete(false), isDrawing(false), figureName(name), currentColor(1.0f, 1.0f, 0.0f) // Default yellow
{
    saveButton = std::make_unique<Button>(10, 10, 120, 30, L"Guardar Figura");
    instructionLabel = std::make_unique<Label>(150, 10, 300, 30, L"Dibuja tu figura");

    // Color palette - 4x5 grid of colors (20 unique colors, rainbow as #20)
    colors = {
        RED, GREEN, BLUE, YELLOW, BLACK,
        CYAN, MAGENTA, GRAY, ORANGE, WHITE,
        LIGHT_BLUE, PINK, DARK_GREEN, BROWN, PURPLE,
        GOLD, SKY_BLUE, LIME, DARK_RED, GetRainbowColor(0.0f)
    };

    // Create color buttons in a 4x5 grid (bottom right area)
    int buttonSize = 25; // Hacer botones más pequeños
    int margin = 5;
    int startX = config.width - (5 * buttonSize + 6 * margin) - 20;  // Más espacio del borde derecho
    int startY = config.height - (4 * buttonSize + 5 * margin) - 60; // Más espacio del borde inferior

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            int index = row * 5 + col;
            if (index < colors.size())
            {
                int x = startX + col * (buttonSize + margin);
                int y = startY + row * (buttonSize + margin);
                auto colorButton = std::make_unique<Button>(x, y, buttonSize, buttonSize, L"R");
                colorButtons.push_back(std::move(colorButton));
            }
        }
    }
}

bool DrawingWindow::Create()
{
    if (!Window::Create())
        return false;

    // Crear elementos UI
    saveButton->Create(GetWindowHandle());
    instructionLabel->Create(GetWindowHandle());

    // Create and configure color buttons
    for (size_t i = 0; i < colorButtons.size() && i < colors.size(); ++i)
    {
        colorButtons[i]->Create(GetWindowHandle());

        // No necesitamos texto para los botones de colores, solo el color visual
        colorButtons[i]->SetText(L""); // Botón sin texto

        // Configure callback for color selection
        Color buttonColor = colors[i];
        colorButtons[i]->SetOnClick([this, buttonColor]()
                                    { OnColorButtonClick(buttonColor); });

        colorButtons[i]->Show();
    }

    // Configurar callback del botón de guardar
    saveButton->SetOnClick([this]()
                           { OnSaveButtonClick(); });

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
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == BN_CLICKED)
        {
            // Verificar qué control específico envió el mensaje
            int controlId = LOWORD(wParam);

            // IDs de los controles de DrawingWindow (basado en orden de creación)
            // saveButton: 1001 (primero)
            // colorButtons: 1002-1021 (20 botones)
            // Nota: rainbowButton removido

            if (controlId == 1001) // saveButton
            {
                OnSaveButtonClick();
            }
            else if (controlId >= 1002 && controlId <= 1021) // colorButtons (20 botones)
            {

                int colorIndex = controlId - 1002;
                if (colorIndex >= 0 && colorIndex < colors.size())
                {
                    OnColorButtonClick(colors[colorIndex]);
                }
            }
        }
        return 0;
    }

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

        auto *renderer = GetRenderer();
        if (renderer)
        {
            // Asegurar que el contexto OpenGL esté activo
            wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

            renderer->Render();
            DrawLines();
            DrawColorPicker();
            // DrawRainbowBox() removed - rainbow functionality now only in color grid
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
    }

    return Window::HandleMessage(hwnd, msg, wParam, lParam);
}

void DrawingWindow::OnMouseClick(int x, int y)
{
    if (figureComplete)
        return;

    HomogenVector glPoint = ScreenToOpenGL(x, y);
    points.push_back(glPoint);

    std::wcout << L"Point added: (" << glPoint.x << L", " << glPoint.y << L")" << std::endl;

    CheckFigureComplete();
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

HomogenVector DrawingWindow::ScreenToOpenGL(int screenX, int screenY)
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

void DrawingWindow::CheckFigureComplete()
{
    if (points.size() < 3)
        return;

    // Verificar si el último punto está cerca del primero (figura cerrada)
    const HomogenVector &first = points[0];
    const HomogenVector &last = points.back();

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
    auto *renderer = GetRenderer();
    if (!renderer)
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    if (points.size() < 2)
        return;

    // Usar el color actual seleccionado
    glColor3f(currentColor.r, currentColor.g, currentColor.b);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);
        glVertex2f(glX, glY);
    }
    glEnd();

    // Dibujar puntos
    glColor3f(currentColor.r, currentColor.g, currentColor.b);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto &point : points)
    {
        float glX, glY;
        point.ToOpenGL(glX, glY);
        glVertex2f(glX, glY);
    }
    glEnd();
}

void DrawingWindow::OnSaveButtonClick()
{
    std::wcout << L"*** SAVE BUTTON CLICKED! ***" << std::endl;
    std::wcout << L"Figure saved with " << points.size() << L" points:" << std::endl;
    for (size_t i = 0; i < points.size(); ++i)
    {
        std::wcout << L"Point " << i << L": (" << points[i].x << L", " << points[i].y << L")" << std::endl;
    }
    std::wcout << L"Figure color: (" << currentColor.r << L", " << currentColor.g << L", " << currentColor.b << L")" << std::endl;

    // Crear figura y notificar al callback
    auto figure = std::make_shared<Figure>(figureName);
    for (const auto &point : points)
    {
        figure->AddPoint(point);
    }
    figure->SetComplete(true);
    figure->SetColor(currentColor);

    // Notificar que la figura está completa
    FigureManager::NotifyFigureComplete(figure);

    // Cerrar la ventana de dibujo
    PostMessage(GetWindowHandle(), WM_CLOSE, 0, 0);
}

void DrawingWindow::OnColorButtonClick(const Color &color)
{
    currentColor = color;

    // Check if the selected color is the rainbow color from the grid
    Color rainbowColor = GetRainbowColor(0.0f);
    if (color.r == rainbowColor.r && color.g == rainbowColor.g && color.b == rainbowColor.b)
    {
        // Rainbow mode selected from grid - set to dynamic rainbow
        std::wcout << L"Rainbow mode activated from color grid!" << std::endl;
        currentColor = GetRainbowColor(static_cast<float>(rand()) / RAND_MAX);
    }
    else
    {
        std::wcout << L"Color selected: (" << color.r << L", " << color.g << L", " << color.b << L")" << std::endl;
    }

    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

void DrawingWindow::DrawColorPicker()
{
    auto *renderer = GetRenderer();
    if (!renderer)
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());


    // Get window dimensions for coordinate conversion
    RECT rect;
    GetClientRect(GetWindowHandle(), &rect);

    // Draw color buttons backgrounds (synchronized with Windows buttons)
    int buttonSize = 25;
    int margin = 5;
    int startX = rect.right - (5 * buttonSize + 6 * margin) - 1;
    int startY = rect.bottom - (4 * buttonSize + 5 * margin) - 150;

    for (size_t i = 0; i < colorButtons.size() && i < colors.size(); ++i)
    {
        Color buttonColor = colors[i];

        // Calculate position for this color button
        int row = i / 5;
        int col = i % 5;
        int x = startX + col * (buttonSize + margin);
        int y = startY + row * (buttonSize + margin);

        // Convert button coordinates to OpenGL
        float glX = (2.0f * x / rect.right) - 1.0f;
        float glY = 1.0f - (2.0f * y / rect.bottom);
        float glWidth = (2.0f * buttonSize / rect.right);
        float glHeight = (2.0f * buttonSize / rect.bottom);

        // Draw filled rectangle
        glColor3f(buttonColor.r, buttonColor.g, buttonColor.b);
        glBegin(GL_QUADS);
        glVertex2f(glX, glY);
        glVertex2f(glX + glWidth, glY);
        glVertex2f(glX + glWidth, glY - glHeight);
        glVertex2f(glX, glY - glHeight);
        glEnd();

        // Draw black border if not selected color
        if (buttonColor.r != currentColor.r || buttonColor.g != currentColor.g || buttonColor.b != currentColor.b)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(glX, glY);
            glVertex2f(glX + glWidth, glY);
            glVertex2f(glX + glWidth, glY - glHeight);
            glVertex2f(glX, glY - glHeight);
            glEnd();
        }
        else
        {
            // Draw white border for selected color
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(glX, glY);
            glVertex2f(glX + glWidth, glY);
            glVertex2f(glX + glWidth, glY - glHeight);
            glVertex2f(glX, glY - glHeight);
            glEnd();
        }
    }
}


void DrawingWindow::ClearDrawing()
{
    points.clear();
    figureComplete = false;
    currentColor = Color(1.0f, 1.0f, 0.0f); // Reset to default yellow
    saveButton->Hide();
    instructionLabel->SetText(L"Dibuja tu figura");
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}
