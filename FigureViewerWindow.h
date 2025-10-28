// FigureViewerWindow.h - Ventana para visualizar múltiples figuras con navegación carrusel
#pragma once
#include "Window.h"
#include "Figure.h"
#include "HomogenVector.h"
#include "Color.h"
#include "Button.h"
#include <memory>
#include <vector>

class FigureViewerWindow : public Window
{
private:
    std::vector<std::shared_ptr<Figure>> figures; // Múltiples figuras para navegación
    size_t currentFigureIndex;                    // Índice de la figura actual
    HomogenVector pivotPoint;                     // Punto pivote para transformaciones
    bool hasPivot;                                // Indica si se ha establecido el pivote
    bool sPressed;
    bool tPressed;
    bool rPressed;
    bool gPressed;

    // Botones de navegación carrusel
    std::unique_ptr<Button> leftButton;
    std::unique_ptr<Button> rightButton;

    void DrawSingleFigure();
    void DrawPivotPoint();
    void HandleClick(int x, int y);
    void HandleKeyboard(WPARAM wParam);
    void UpdateKeyState(WPARAM wParam, bool pressed);
    void ClearKeyState();
    void UpdateButtonVisibility();
    void UpdateWindowTitle();

    // Funciones de transformación
    void scalar_x(bool increase);
    void scalar_y(bool increase);
    void rotar_left();
    void rotar_right();
    void trasladar_x(bool right);
    void trasladar_y(bool up);

    // NUEVAS: Funciones de transformación combinada
    void scalar_trasladar_x(bool right);
    void scalar_trasladar_y(bool up);
    void trasladar_rotar_left();
    void trasladar_rotar_right();

    // NUEVAS: Funciones de navegación y redibujo
    void NavigateToPreviousFigure();
    void NavigateToNextFigure();
    void RedrawCurrentFigure();
    void RedrawWithNewFigure();
    void event_g();

    // Callbacks para botones de navegación
    void OnLeftButtonClick();
    void OnRightButtonClick();

    void PrintFigurePoints(const std::string &eventName);
    HomogenVector ScreenToOpenGL(int screenX, int screenY);

public:
    FigureViewerWindow(const WindowConfig &config, const std::vector<std::shared_ptr<Figure>> &figuresToView);
    ~FigureViewerWindow() = default;

    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
