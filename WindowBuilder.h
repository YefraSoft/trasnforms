// WindowBuilder.h - Builder Pattern + SOLID
#pragma once
#include "Window.h"
#include "WindowCommunicator.h"
#include <memory>
#include <vector>

class WindowBuilder
{
private:
    std::vector<std::shared_ptr<Window>> windows;
    std::shared_ptr<WindowCommunicator> communicator;

public:
    WindowBuilder();

    std::shared_ptr<Window> Build(const std::wstring &title, int width, int height);
    std::shared_ptr<Window> Build(const WindowConfig &config);

    std::shared_ptr<WindowCommunicator> GetCommunicator() const;
    const std::vector<std::shared_ptr<Window>> &GetWindows() const;

    void MessageLoop();
};