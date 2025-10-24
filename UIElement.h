// UIElement.h - Base class for UI elements
#pragma once
#include <windows.h>
#include <string>

class UIElement
{
protected:
    HWND hwnd;
    int x, y, width, height;
    std::wstring text;

public:
    UIElement(int x, int y, int w, int h, const std::wstring& text);
    virtual ~UIElement() = default;
    
    virtual bool Create(HWND parent) = 0;
    virtual void Show();
    virtual void Hide();
    virtual void SetText(const std::wstring& newText);
    HWND GetHandle() const { return hwnd; }
};
