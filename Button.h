// Button.h - Button UI element
#pragma once
#include "UIElement.h"
#include <functional>

class Button : public UIElement
{
private:
    std::function<void()> onClickCallback;

public:
    Button(int x, int y, int w, int h, const std::wstring& text);
    
    bool Create(HWND parent) override;
    void SetOnClick(std::function<void()> callback);
    
    static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
