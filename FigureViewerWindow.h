// FigureViewerWindow.h - Ventana para visualizar una sola figura
#pragma once
#include "Window.h"
#include "Figure.h"
#include "HomogenVector.h"
#include "Color.h"
#include <memory>

class FigureViewerWindow : public Window
{
private:
    std::shared_ptr<Figure> figure;
    HomogenVector pivotPoint;  // Punto pivote para transformaciones
    bool hasPivot;             // Indica si se ha establecido el pivote

    void DrawSingleFigure();
    void DrawPivotPoint();
    void HandleClick(int x, int y);
    void HandleKeyboard(WPARAM wParam);

    // Funciones de transformación
    void scalar_x(bool increase);
    void scalar_y(bool increase);
    void rotar_left();
    void rotar_right();
    void trasladar_x(bool right);
    void trasladar_y(bool up);

    void PrintFigurePoints(const std::string& eventName);
    HomogenVector ScreenToOpenGL(int screenX, int screenY);

public:
    FigureViewerWindow(const WindowConfig& config, std::shared_ptr<Figure> figureToView);
    ~FigureViewerWindow() = default;

    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
