// Label.h - Label UI element
#pragma once
#include "UIElement.h"

class Label : public UIElement
{
public:
    Label(int x, int y, int w, int h, const std::wstring& text);
    
    bool Create(HWND parent) override;
};
