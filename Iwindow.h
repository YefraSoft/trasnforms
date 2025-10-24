// IWindow.h - Interface Segregation Principle
#pragma once
#include <windows.h>
#include <string>

class IWindow
{
public:
    virtual ~IWindow() = default;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void SetTitle(const wchar_t *title) = 0;
    virtual HWND GetHandle() const = 0;
    virtual bool IsActive() const = 0;
};