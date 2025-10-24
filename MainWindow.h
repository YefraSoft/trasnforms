// MainWindow.h - Main window with UI
#pragma once
#include "Window.h"
#include "Button.h"
#include "Label.h"
#include "DrawingWindow.h"
#include "Figure.h"
#include "FigureCallback.h"
#include "Color.h"
#include <memory>
#include <vector>

class MainWindow : public Window
{
private:
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<Button> drawButton;
    std::vector<std::shared_ptr<Figure>> figures;
    std::vector<std::unique_ptr<DrawingWindow>> drawingWindows;
    int figureCounter;
    
    void OnDrawButtonClick();
    void OnFigureComplete(std::shared_ptr<Figure> figure);
    void DrawAllFigures();

public:
    MainWindow(const WindowConfig& config);
    ~MainWindow() = default;
    
    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    bool HasActiveWindows() const;
};
