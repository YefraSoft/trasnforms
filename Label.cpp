// Label.cpp
#include "Label.h"

Label::Label(int x, int y, int w, int h, const std::wstring& text)
    : UIElement(x, y, w, h, text)
{
}

bool Label::Create(HWND parent)
{
    hwnd = CreateWindowW(
        L"STATIC",
        text.c_str(),
        WS_VISIBLE | WS_CHILD | SS_LEFT | SS_NOTIFY,
        x, y, width, height,
        parent,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (hwnd)
    {
        // Configurar fuente para mejor visibilidad
        HFONT hFont = CreateFontW(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial"
        );
        SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
        
        // Forzar redibujado del label
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);
    }
    
    return hwnd != nullptr;
}
