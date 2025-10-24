// MainWindow.h - Main window with UI
#pragma once
#include "Window.h"
#include "Button.h"
#include "Label.h"
#include "DrawingWindow.h"
#include <memory>

class MainWindow : public Window
{
private:
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<Button> drawButton;
    std::unique_ptr<DrawingWindow> drawingWindow;
    
    void OnDrawButtonClick();

public:
    MainWindow(const WindowConfig& config);
    ~MainWindow() = default;
    
    bool Create() override;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
