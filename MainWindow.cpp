// MainWindow.cpp
#include "MainWindow.h"
#include <iostream>
#include <algorithm> // Para std::min y std::max

MainWindow::MainWindow(const WindowConfig &config)
    : Window(config), figureCounter(0)
{
    titleLabel = std::make_unique<Label>(250, 30, 500, 30, L"Transformaciones Geométricas");
    drawButton = std::make_unique<Button>(260, 70, 150, 40, L"Abrir Dibujo");
    viewButton = std::make_unique<Button>(420, 70, 150, 40, L"Ver Primera");
}

bool MainWindow::Create()
{
    if (!Window::Create())
        return false;

    // Crear elementos UI
    titleLabel->Create(GetWindowHandle());
    drawButton->Create(GetWindowHandle());
    viewButton->Create(GetWindowHandle());

    // Configurar callback del botón
    drawButton->SetOnClick([this]()
                           { OnDrawButtonClick(); });

    // Configurar callback del botón de vista
    viewButton->SetOnClick([this]()
                           { OnViewButtonClick(); });

    // Configurar callback global para figuras
    FigureManager::SetGlobalCallback([this](std::shared_ptr<Figure> figure)
                                     { OnFigureComplete(figure); });

    // Forzar redibujado de los controles
    titleLabel->Show();
    drawButton->Show();
    // viewButton oculto inicialmente hasta que haya figuras
    viewButton->Hide();
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
            // Verificar qué control específico envió el mensaje
            int controlId = LOWORD(wParam);

            // ID del botón de dibujo (primero que se crea)
            if (controlId == 1001) // ID del drawButton
            {
                OnDrawButtonClick();
            }
            else if (controlId == 1002) // ID del viewButton
            {
                OnViewButtonClick();
            }
            // Para otros controles, ignorar por ahora
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

    // Verificar si alguna ventana de vista está activa
    for (const auto &viewerWindow : viewerWindows)
    {
        if (viewerWindow && viewerWindow->IsActive())
            return true;
    }

    return false;
}

void MainWindow::OnViewButtonClick()
{
    std::wcout << L"Opening figure viewer..." << std::endl;

    // Verificar si hay figuras disponibles
    if (figures.empty())
    {
        std::wcout << L"No figures available to view." << std::endl;
        return;
    }

    // Verificar si ya hay ventanas de vista activas
    int activeViewerWindows = 0;
    for (const auto &viewerWindow : viewerWindows)
    {
        if (viewerWindow && viewerWindow->IsActive())
            activeViewerWindows++;
    }

    if (activeViewerWindows > 0)
    {
        std::wcout << L"Warning: There is already an active viewer window. Please close it first." << std::endl;
        return;
    }

    // Obtener la primera figura
    auto firstFigure = figures[0];
    std::wcout << L"Viewing first figure: " << firstFigure->GetName().c_str() << std::endl;

    // Crear ventana de vista con la primera figura
    WindowConfig viewConfig(L"Figure Viewer", 600, 500, 300, 200);
    auto viewerWindow = std::make_unique<FigureViewerWindow>(viewConfig, firstFigure);

    if (viewerWindow->Create())
    {
        viewerWindow->Show();

        // Mantener z-order: ventana de vista detrás de la principal
        SetWindowPos(
            viewerWindow->GetWindowHandle(),       // Ventana de vista
            GetWindowHandle(),                     // Ventana principal (referencia)
            0, 0, 0, 0,                            // No cambiar posición/tamaño
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER // Mantener z-order actual
        );

        // Almacenar la ventana para que no se destruya
        viewerWindows.push_back(std::move(viewerWindow));
        std::wcout << L"Figure viewer window created successfully" << std::endl;
    }
    else
    {
        std::wcout << L"Failed to create figure viewer window" << std::endl;
    }
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

        // Mantener z-order: ventana de dibujo detrás de la principal
        // para evitar que se superponga y tape la interfaz principal
        SetWindowPos(
            drawingWindow->GetWindowHandle(),      // Ventana de dibujo
            GetWindowHandle(),                     // Ventana principal (referencia)
            0, 0, 0, 0,                            // No cambiar posición/tamaño
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER // Mantener z-order actual
        );

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

    // Mostrar botón "Ver Primera" si es la primera figura
    if (figures.size() == 1)
    {
        viewButton->Show();
        UpdateWindow(GetWindowHandle());
        std::wcout << L"DEBUG: View button shown - first figure available" << std::endl;
    }

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
    // ============================================================================
    // NUEVO MÉTODO DE ORGANIZACIÓN DE FIGURAS - SISTEMA DE GRID SIMPLE
    // ============================================================================
    //
    // Sistema mejorado: Organización en grid con posiciones fijas
    //
    // Ventajas del nuevo sistema:
    // 1. Posiciones predecibles y fijas basadas en índice de figura
    // 2. Sin cálculos de offset acumulativos (no hay error de acumulación)
    // 3. Grid responsive que se adapta al número de figuras
    // 4. Escaling uniforme para todas las figuras
    // 5. Sin debug output innecesario
    // 6. Código más limpio y mantenible
    // 7. Mejor uso del espacio disponible
    //
    // Configuración del Grid:
    // - Máximo 6 figuras por fila (ajustable)
    // - Área de dibujo: -0.9f a 0.9f (horizontal y vertical)
    // - Espacio entre figuras: 10% del área total
    // - Escaling automático basado en el número de figuras
    //
    // ============================================================================

    auto *renderer = GetRenderer();
    if (!renderer || figures.empty())
        return;

    // Asegurar que el contexto OpenGL esté activo
    wglMakeCurrent(GetDC(GetWindowHandle()), renderer->GetGLRC());

    // Configuración del área de dibujo
    const float areaLeft = -0.9f;
    const float areaRight = 0.9f;
    const float areaTop = 0.9f;
    const float areaBottom = -0.9f;

    const int maxFiguresPerRow = 3; // Máximo 3 figuras por fila
    const int maxRows = 3;          // Máximo 3 filas (9 figuras total)

    // Calcular grid dimensions
    int totalFigures = static_cast<int>(figures.size());
    int figuresPerRow = (std::min)(maxFiguresPerRow, totalFigures);
    int totalRows = (totalFigures + figuresPerRow - 1) / figuresPerRow; // Ceiling division
    totalRows = (std::min)(totalRows, maxRows);

    // Calcular tamaño de celda
    float cellWidth = (areaRight - areaLeft) / figuresPerRow;
    float cellHeight = (areaTop - areaBottom) / totalRows;

    // Dibujar cada figura en su posición de grid
    for (int i = 0; i < totalFigures; ++i)
    {
        const auto &figure = figures[i];
        const auto &points = figure->GetPoints();

        if (points.size() < 2)
            continue;

        // Calcular posición en el grid
        int row = i / figuresPerRow;
        int col = i % figuresPerRow;

        // Calcular centro de la celda
        float cellCenterX = areaLeft + (col + 0.5f) * cellWidth;
        float cellCenterY = areaTop - (row + 0.5f) * cellHeight;

        // Calcular tamaño de la figura para escalar
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

        // Calcular escala para que quepa en la celda (con margen del 10%)
        float scaleX = (cellWidth * 0.9f) / figureWidth;
        float scaleY = (cellHeight * 0.9f) / figureHeight;
        float scale = (std::min)(scaleX, scaleY);

        // Si la figura es muy pequeña, usar escala 1:1 con límite superior
        if (scale > 1.0f)
            scale = 1.0f;

        // Usar el color original de la figura
        Color figureColor = figure->GetColor();
        glColor3f(figureColor.r, figureColor.g, figureColor.b);
        glLineWidth(2.0f);

        // Dibujar línea de la figura
        glBegin(GL_LINE_STRIP);
        for (const auto &point : points)
        {
            float glX, glY;
            point.ToOpenGL(glX, glY);

            // Aplicar escala y centrar en la celda
            glX = cellCenterX + (glX - (minX + maxX) / 2.0f) * scale;
            glY = cellCenterY + (glY - (minY + maxY) / 2.0f) * scale;

            glVertex2f(glX, glY);
        }
        glEnd();

        // Dibujar puntos de la figura
        glColor3f(figureColor.r, figureColor.g, figureColor.b);
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        for (const auto &point : points)
        {
            float glX, glY;
            point.ToOpenGL(glX, glY);

            // Aplicar escala y centrar en la celda
            glX = cellCenterX + (glX - (minX + maxX) / 2.0f) * scale;
            glY = cellCenterY + (glY - (minY + maxY) / 2.0f) * scale;

            glVertex2f(glX, glY);
        }
        glEnd();
    }
}
