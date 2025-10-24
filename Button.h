// Button.h - Button UI element
#pragma once
#include "UIElement.h"
#include <functional>

class Button : public UIElement
{
private:
    std::function<void()> onClickCallback;
    WNDPROC originalWndProc; // Store original window procedure

public:
    Button(int x, int y, int w, int h, const std::wstring& text);

    bool Create(HWND parent) override;
    void SetOnClick(std::function<void()> callback);

    // Getter methods for position and size
    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

    static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
