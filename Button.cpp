// Button.cpp
#include "Button.h"

Button::Button(int x, int y, int w, int h, const std::wstring& text)
    : UIElement(x, y, w, h, text)
{
}

bool Button::Create(HWND parent)
{
    static int buttonId = 1000;
    buttonId++;
    
    hwnd = CreateWindowW(
        L"BUTTON",
        text.c_str(),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        x, y, width, height,
        parent,
        reinterpret_cast<HMENU>(buttonId),
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (hwnd)
    {
        // Forzar redibujado del botón
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);
    }
    
    return hwnd != nullptr;
}

void Button::SetOnClick(std::function<void()> callback)
{
    onClickCallback = callback;
}

LRESULT CALLBACK Button::ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
    {
        Button* button = reinterpret_cast<Button*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (button && button->onClickCallback)
        {
            button->onClickCallback();
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
