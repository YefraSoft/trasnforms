// UIElement.cpp
#include "UIElement.h"

UIElement::UIElement(int x, int y, int w, int h, const std::wstring& text)
    : hwnd(nullptr), x(x), y(y), width(w), height(h), text(text)
{
}

void UIElement::Show()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_SHOW);
    }
}

void UIElement::Hide()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_HIDE);
    }
}

void UIElement::SetText(const std::wstring& newText)
{
    text = newText;
    if (hwnd)
    {
        SetWindowTextW(hwnd, text.c_str());
    }
}
