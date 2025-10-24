// DrawingWindow.h - Window for drawing figures
#pragma once
#include "Window.h"
#include "OpenGLRenderer.h"
#include "Button.h"
#include "Label.h"
#include <vector>
#include <memory>

struct Point2D
{
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
};

class DrawingWindow : public Window
{
private:
    std::vector<Point2D> points;
    std::unique_ptr<Button> saveButton;
    std::unique_ptr<Label> instructionLabel;
    bool figureComplete;
    bool isDrawing;
    
    void OnMouseClick(int x, int y);
    void OnSaveButtonClick();
    void CheckFigureComplete();
    void DrawLines();
    Point2D ScreenToOpenGL(int screenX, int screenY);

public:
    DrawingWindow(const WindowConfig& config);
    ~DrawingWindow() = default;
    
    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
    
    std::vector<Point2D> GetPoints() const { return points; }
    void ClearDrawing();
};
