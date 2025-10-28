// FigureViewerWindow.cpp
#include "FigureViewerWindow.h"
#include <iostream>
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

void FigureViewerWindow::UpdateWindowTitle()
{
    if (!figures.empty() && currentFigureIndex < figures.size())
    {
        std::string title = "Figure Viewer - " + figures[currentFigureIndex]->GetName() +
                            " (" + std::to_string(currentFigureIndex + 1) + "/" + std::to_string(figures.size()) + ")" +
                            " | Click pivote | <- -> navegar | S/T/R/G + flechas | S+T/T+R combinadas | T+→ redibujar";
        std::wstring wTitle(title.begin(), title.end());
        SetTitle(wTitle.c_str());
    }
    else
    {
        SetTitle(L"Figure Viewer - No Figures | Click para pivote");
    }
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
        std::wcout << L"[WM_KEYDOWN] wParam=" << wParam << L" Focus=0x" 
               << std::hex << (intptr_t)GetFocus() << std::dec << std::endl;
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

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    const auto &points = figure->GetPoints();
    if (points.size() < 2)
        return;

    // Obtener color de la figura
    Color figureColor = figure->GetColor();
    std::wcout << L"FigureViewer: Rendering figure " << figure->GetName().c_str()
               << L" with color (" << figureColor.r << L", " << figureColor.g << L", " << figureColor.b << L")" << std::endl;

    // Configuración del área de dibujo (dejar espacio para botones en la parte superior)
    const float areaLeft = -0.8f;
    const float areaRight = 0.8f;
    const float areaTop = 0.7f; // Dejar espacio para botones
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
    // No dependemos de sPressed/tPressed para decisiones inmediatas.
    // Usamos GetAsyncKeyState para saber si el modificador está down justo ahora.
    auto isDown = [](int vk) -> bool
    {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    };

    bool sDown = isDown('S');
    bool tDown = isDown('T');
    bool rDown = isDown('R');
    bool gDown = isDown('G');

    switch (wParam)
    {
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(), WM_CLOSE, 0, 0);
        break;

    case 'G':
        event_g();
        break;

    case VK_RIGHT:
        if (tDown)
        {
           RedrawWithNewFigure();
            
            break;
        }

        if (sDown && tDown)
        {
            PrintFigurePoints("scalar_trasladar_x");
            std::wcout << L"S+T+→ detectado" << std::endl;
        }
        else if (tDown && rDown)
        {
            PrintFigurePoints("trasladar_rotar");
            std::wcout << L"T+R+→ detectado" << std::endl;
        }
        else if (sDown)
            scalar_x(true);
        else if (rDown)
            rotar_right();
        else if (tDown)
            trasladar_x(true);
        else
            NavigateToNextFigure();
        break;

    case VK_LEFT:
        if (sDown && tDown)
        {
            PrintFigurePoints("scalar_trasladar_x");
        }
        else if (tDown && rDown)
        {
            PrintFigurePoints("trasladar_rotar");
        }
        else if (sDown)
            scalar_x(false);
        else if (rDown)
            rotar_left();
        else if (tDown)
            trasladar_x(false);
        else
            NavigateToPreviousFigure();
        break;

    case VK_UP:
        if (sDown && tDown)
        {
            PrintFigurePoints("scalar_trasladar_y");
        }
        else if (sDown)
            scalar_y(true);
        else if (tDown)
            trasladar_y(true);
        break;

    case VK_DOWN:
        if (sDown && tDown)
        {
            PrintFigurePoints("scalar_trasladar_y");
        }
        else if (sDown)
            scalar_y(false);
        else if (tDown)
            trasladar_y(false);
        break;
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

void FigureViewerWindow::PrintFigurePoints(const std::string &eventName)
{
    if (figures.empty() || currentFigureIndex >= figures.size())
        return;

    auto figure = figures[currentFigureIndex];
    if (!figure)
        return;

    std::wcout << L"=== " << eventName.c_str() << L" ===" << std::endl;
    std::wcout << L"Current figure: " << figure->GetName().c_str()
               << L" (" << (currentFigureIndex + 1) << L"/" << figures.size() << L")" << std::endl;

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

// NUEVO: Evento individual 'g'
void FigureViewerWindow::event_g()
{
    PrintFigurePoints("event_g");
    std::wcout << L"Evento 'event_g' detectado" << std::endl;
}

// NUEVAS: Funciones de navegación carrusel
void FigureViewerWindow::NavigateToPreviousFigure()
{
    if (figures.size() <= 1)
        return;

    // Navegación circular: si está en la primera, ir a la última
    if (currentFigureIndex == 0)
        currentFigureIndex = figures.size() - 1;
    else
        currentFigureIndex--;

    // Limpiar pivote cuando se cambia de figura
    hasPivot = false;

    // NO limpiar estado del teclado - dejar que el usuario continúe con las transformaciones
    ClearKeyState(); // ← REMOVED: No limpiar estado para permitir eventos continuos

    std::wcout << L"Navigated to previous figure: " << figures[currentFigureIndex]->GetName().c_str()
               << L" (" << (currentFigureIndex + 1) << L"/" << figures.size() << L")" << std::endl;

    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

void FigureViewerWindow::NavigateToNextFigure()
{
    if (figures.size() <= 1)
        return;

    // Navegación circular: si está en la última, ir a la primera
    if (currentFigureIndex >= figures.size() - 1)
        currentFigureIndex = 0;
    else
        currentFigureIndex++;

    // Limpiar pivote cuando se cambia de figura
    hasPivot = false;

    // NO limpiar estado del teclado - dejar que el usuario continúe con las transformaciones
    ClearKeyState(); // ← REMOVED: No limpiar estado para permitir eventos continuos

    std::wcout << L"Navigated to next figure: " << figures[currentFigureIndex]->GetName().c_str()
               << L" (" << (currentFigureIndex + 1) << L"/" << figures.size() << L")" << std::endl;

    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

void FigureViewerWindow::RedrawCurrentFigure()
{
    if (figures.empty() || currentFigureIndex >= figures.size())
        return;

    PrintFigurePoints("redraw_current");
    std::wcout << L"Redrawing current figure: " << figures[currentFigureIndex]->GetName().c_str() << std::endl;
    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
}

// FUNCIÓN PRINCIPAL: RedrawWithNewFigure() - Función detallada de ejemplo
// Esta función demuestra cómo limpiar y redibujar una figura después de transformaciones
void FigureViewerWindow::RedrawWithNewFigure()
{
    std::wcout << L"=== RedrawWithNewFigure() EJEMPLO DETALLADO ===" << std::endl;
    std::wcout << L"Función de ejemplo para redibujar figuras después de transformaciones" << std::endl;

    if (figures.empty() || currentFigureIndex >= figures.size())
    {
        std::wcout << L"ERROR: No hay figuras disponibles para redibujar" << std::endl;
        return;
    }

    auto currentFigure = figures[currentFigureIndex];
    std::wcout << L"Procesando figura actual: " << currentFigure->GetName().c_str() << std::endl;

    // PASO 1: Limpiar estado actual
    std::wcout << L"PASO 1: Limpiando estado actual..." << std::endl;
    hasPivot = false;                          // Limpiar punto pivote
    pivotPoint = HomogenVector(0.0f, 0.0f, 1); // Resetear punto pivote

    // PASO 2: Obtener información de la figura actual
    std::wcout << L"PASO 2: Analizando figura actual..." << std::endl;
    const auto &points = currentFigure->GetPoints();
    Color figureColor = currentFigure->GetColor();

    std::wcout << L"  - Puntos totales: " << points.size() << std::endl;
    std::wcout << L"  - Color actual: (" << figureColor.r << L", " << figureColor.g << L", " << figureColor.b << L")" << std::endl;

    // PASO 3: Crear nueva figura basada en la actual (ejemplo inventado)
    std::wcout << L"PASO 3: Creando nueva figura basada en la actual..." << std::endl;

    // Ejemplo: Crear un cuadrado basado en los límites de la figura actual
    if (points.size() >= 2)
    {
        // Calcular límites de la figura actual
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

        std::wcout << L"  - Límites calculados: (" << minX << L", " << minY << L") to (" << maxX << L", " << maxY << L")" << std::endl;

        // Crear un cuadrado que encierre la figura actual
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;
        float size = (std::max)(maxX - minX, maxY - minY) * 0.7f;

        std::wcout << L"  - Centro calculado: (" << centerX << L", " << centerY << L")" << std::endl;
        std::wcout << L"  - Tamaño del cuadrado: " << size << std::endl;
    }

    // PASO 4: Aplicar transformación de color a la figura (siempre)
    std::wcout << L"PASO 4: Aplicando transformación de color a la figura..." << std::endl;

    // Aplicar una transformación visible: cambiar el color a su complemento
    Color newColor(1.0f - figureColor.r, 1.0f - figureColor.g, 1.0f - figureColor.b);

    std::wcout << L"  - Color original: (" << figureColor.r << L", " << figureColor.g << L", " << figureColor.b << L")" << std::endl;
    std::wcout << L"  - Nuevo color: (" << newColor.r << L", " << newColor.g << L", " << newColor.b << L")" << std::endl;

    // APLICAR LA TRANSFORMACIÓN: Cambiar el color de la figura actual
    currentFigure->SetColor(newColor);
    std::wcout << L"  - Transformación de color aplicada exitosamente" << std::endl;

    // PASO 5: Finalizar transformación y actualizar interfaz
    std::wcout << L"PASO 5: Finalizando transformación..." << std::endl; // Actualizar título con nueva información                 // Forzar redibujado completo

    // PASO 6: Log detallado del proceso completado
    std::wcout << L"PASO 6: Transformación completada exitosamente" << std::endl;
    std::wcout << L"  - Color de figura transformado" << std::endl;
    std::wcout << L"  - Estado de pivote: " << (hasPivot ? L"activo" : L"inactivo") << std::endl;
    std::wcout << L"  - Ventana actualizada y lista para nuevas operaciones" << std::endl;

    InvalidateRect(GetWindowHandle(), nullptr, FALSE);
    PrintFigurePoints("redraw_complete");
    std::wcout << L"Evento 'RedrawWithNewFigure' completado - T+→ ejecutado exitosamente" << std::endl;
    std::wcout << L"=====================================" << std::endl;
    ClearKeyState();
    SetFocus(GetWindowHandle());
    BringWindowToTop(GetWindowHandle());

    // Asegurar un repintado inmediato (opcional: solo para pruebas)
    UpdateWindow(GetWindowHandle());
    std::wcout << L"After Redraw: Focus=0x" << std::hex << (intptr_t)GetFocus() << std::dec << std::endl;
}

// Callbacks para botones de navegación
void FigureViewerWindow::OnLeftButtonClick()
{
    std::wcout << L"Left button clicked - navigating to previous figure" << std::endl;
    NavigateToPreviousFigure();
}

void FigureViewerWindow::OnRightButtonClick()
{
    std::wcout << L"Right button clicked - navigating to next figure" << std::endl;
    NavigateToNextFigure();
}

// NUEVAS: Funciones para manejo de estado de teclado
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
    case 'G':
        gPressed = pressed;
        break;
    }
}

void FigureViewerWindow::ClearKeyState()
{
    sPressed = false;
    tPressed = false;
    rPressed = false;
    gPressed = false;
}
