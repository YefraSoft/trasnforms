// WindowCommunicator.h - Single Responsibility & Dependency Inversion
#pragma once
#include "IWindowCommunicator.h"
#include <vector>
#include <memory>
#include <functional>

class WindowCommunicator : public IWindowCommunicator
{
private:
    std::vector<std::function<void(const std::wstring &)>> listeners;

public:
    void Send(const std::wstring &message) override;
    void BroadcastMessage(const std::wstring &message) override;
    void RegisterListener(std::function<void(const std::wstring &)> listener);
};