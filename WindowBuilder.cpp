// WindowBuilder.cpp
#include "WindowBuilder.h"

WindowBuilder::WindowBuilder()
{
    communicator = std::make_shared<WindowCommunicator>();
}

std::shared_ptr<Window> WindowBuilder::Build(const std::wstring &title, int width, int height)
{
    WindowConfig config(title.c_str(), width, height);
    return Build(config);
}

std::shared_ptr<Window> WindowBuilder::Build(const WindowConfig &config)
{
    auto window = std::make_shared<Window>(config);

    if (window->Create())
    {
        windows.push_back(window);

        // Registrar listener para comunicación entre ventanas
        communicator->RegisterListener([window](const std::wstring &msg) {
            if (window->IsActive() && window->GetHandle()) {
                // Preservar el título original y agregar el mensaje
                std::wstring originalTitle = window->GetConfig().title;
                std::wstring newTitle = originalTitle + L" - " + msg;
                
                // Debug: verificar que el título original se preserve
                if (originalTitle.empty()) {
                    newTitle = L"Window - " + msg;
                }
                
                window->SetTitle(newTitle.c_str());
            }
        });

        return window;
    }

    return nullptr;
}

std::shared_ptr<WindowCommunicator> WindowBuilder::GetCommunicator() const
{
    return communicator;
}

const std::vector<std::shared_ptr<Window>> &WindowBuilder::GetWindows() const
{
    return windows;
}

void WindowBuilder::MessageLoop()
{
    MSG msg = {};

    while (true)
    {
        // Verificar si todas las ventanas están cerradas
        bool anyActive = false;
        for (const auto &window : windows)
        {
            if (window->IsActive())
            {
                anyActive = true;
                break;
            }
        }

        if (!anyActive)
            break;

        // Procesar mensajes
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Solo invalidar ventanas que necesiten redibujado
            // Removemos la invalidación constante para evitar parpadeo
            Sleep(16); // ~60 FPS, reduce CPU usage significativamente
        }
    }
}