// WindowConfig.h - Single Responsibility Principle
#pragma once

#include <string>

struct WindowConfig
{
    std::wstring title;
    int width;
    int height;
    int posX;
    int posY;

    WindowConfig(const wchar_t *t, int w, int h, int x = 0, int y = 0)
        : title(t ? t : L""), width(w), height(h), posX(x), posY(y) {}
        
    WindowConfig(const std::wstring& t, int w, int h, int x = 0, int y = 0)
        : title(t), width(w), height(h), posX(x), posY(y) {}
};