// Window.h - Open/Closed Principle
#pragma once
#include "IWindow.h"
#include "IMessageHandler.h"
#include "WindowConfig.h"
#include "OpenGLRenderer.h"
#include <memory>

class Window : public IWindow, public IMessageHandler
{
private:
    HWND hwnd;
    WindowConfig config;
    std::unique_ptr<OpenGLRenderer> renderer;
    bool active;

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    Window(const WindowConfig &cfg);
    virtual ~Window();

    bool Create();

    // IWindow interface
    void Show() override;
    void Hide() override;
    void SetTitle(const wchar_t *title) override;
    HWND GetHandle() const override;
    bool IsActive() const override;

    // IMessageHandler interface
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

    void SetRenderColor(float r, float g, float b);
    const WindowConfig& GetConfig() const;
};