// DrawingWindow.h - Window for drawing figures
#pragma once
#include "Window.h"
#include "OpenGLRenderer.h"
#include "Button.h"
#include "Label.h"
#include "HomogenVector.h"
#include "Figure.h"
#include "Color.h"
#include <vector>
#include <memory>

class DrawingWindow : public Window
{
private:
    std::vector<HomogenVector> points;
    std::unique_ptr<Button> saveButton;
    std::unique_ptr<Label> instructionLabel;
    std::vector<Color> colors;
    bool figureComplete;
    bool isDrawing;
    std::string figureName;

    // Color picker functionality
    Color currentColor;
    std::vector<std::unique_ptr<Button>> colorButtons;

    void OnMouseClick(int x, int y);
    void OnSaveButtonClick();
    void OnColorButtonClick(const Color& color);
    void CheckFigureComplete();
    void DrawLines();
    void DrawColorPicker();
    HomogenVector ScreenToOpenGL(int screenX, int screenY);

public:
    DrawingWindow(const WindowConfig& config, const std::string& name = "Figure");
    ~DrawingWindow() = default;

    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    std::vector<HomogenVector> GetPoints() const { return points; }
    Color GetCurrentColor() const { return currentColor; }
    void ClearDrawing();
    void SetFigureName(const std::string& name) { figureName = name; }
};
