// IWindowCommunicator.h - Dependency Inversion Principle
#pragma once
#include <string>

class IWindowCommunicator
{
public:
    virtual ~IWindowCommunicator() = default;
    virtual void Send(const std::wstring &message) = 0;
    virtual void BroadcastMessage(const std::wstring &message) = 0;
};
