// IMessageHandler.h - Interface Segregation Principle
#pragma once
#include <windows.h>

class IMessageHandler
{
public:
    virtual ~IMessageHandler() = default;
    virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};