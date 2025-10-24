// WindowCommunicator.cpp
#include "WindowCommunicator.h"

void WindowCommunicator::Send(const std::wstring &message)
{
    if (!listeners.empty())
    {
        listeners[0](message);
    }
}

void WindowCommunicator::BroadcastMessage(const std::wstring &message)
{
    for (auto &listener : listeners)
    {
        listener(message);
    }
}

void WindowCommunicator::RegisterListener(std::function<void(const std::wstring &)> listener)
{
    listeners.push_back(listener);
}