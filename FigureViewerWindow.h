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
    const double PI = 3.14159265358979323846;
    const float TRANSLATE_STEP = 0.02f;
    const float SCALE_FACTOR = 1.01f; 
    float ROTATION_STEP = 0.0f;
    void rotate(float degree);
    void traslate(float tx, float ty);
    void scale(float sx, float sy);
    HomogenVector matrix_prod(float ma[3][3], HomogenVector mb);

    std::vector<std::shared_ptr<Figure>> figures;
    size_t currentFigureIndex;
    HomogenVector pivotPoint;
    bool hasPivot;
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
 
    void NavigateToPreviousFigure();
    void NavigateToNextFigure();

    void OnLeftButtonClick();
    void OnRightButtonClick();

    HomogenVector ScreenToOpenGL(int screenX, int screenY);

public:
    FigureViewerWindow(const WindowConfig &config, const std::vector<std::shared_ptr<Figure>> &figuresToView);
    ~FigureViewerWindow() = default;

    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
