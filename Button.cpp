// Button.cpp
#include "Button.h"

Button::Button(int x, int y, int w, int h, const std::wstring& text)
    : UIElement(x, y, w, h, text), originalWndProc(nullptr)
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
        reinterpret_cast<HMENU>(static_cast<UINT_PTR>(buttonId)),
        GetModuleHandle(nullptr),
        this  // Pasar 'this' como lpParam para que ButtonProc pueda acceder al objeto
    );
    
    if (hwnd)
    {
        // Store the original window procedure before replacing it
        originalWndProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));

        // Asignar ButtonProc como el procedimiento de ventana del botón
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ButtonProc));

        // Almacenar el puntero 'this' en GWLP_USERDATA para que ButtonProc pueda acceder al objeto
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

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
    Button* button = reinterpret_cast<Button*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        // Ejecutar callback cuando el botón es clickeado
        if (button && button->onClickCallback)
        {
            button->onClickCallback();
        }
        break;
    }
    case WM_COMMAND:
    {
        // Si por alguna razón llega aquí, también manejar el click
        if (HIWORD(wParam) == BN_CLICKED && button && button->onClickCallback)
        {
            button->onClickCallback();
        }
        break;
    }
    }

    // Call the original window procedure for unhandled messages
    if (button && button->originalWndProc)
    {
        return CallWindowProc(button->originalWndProc, hwnd, msg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
