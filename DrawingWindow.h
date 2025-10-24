// DrawingWindow.h - Window for drawing figures
#pragma once
#include "Window.h"
#include "OpenGLRenderer.h"
#include "Button.h"
#include "Label.h"
#include "HomogenVector.h"
#include "Figure.h"
#include <vector>
#include <memory>

class DrawingWindow : public Window
{
private:
    std::vector<HomogenVector> points;
    std::unique_ptr<Button> saveButton;
    std::unique_ptr<Label> instructionLabel;
    bool figureComplete;
    bool isDrawing;
    std::string figureName;
    
    void OnMouseClick(int x, int y);
    void OnSaveButtonClick();
    void CheckFigureComplete();
    void DrawLines();
    HomogenVector ScreenToOpenGL(int screenX, int screenY);

public:
    DrawingWindow(const WindowConfig& config, const std::string& name = "Figure");
    ~DrawingWindow() = default;
    
    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
    
    std::vector<HomogenVector> GetPoints() const { return points; }
    void ClearDrawing();
    void SetFigureName(const std::string& name) { figureName = name; }
};
