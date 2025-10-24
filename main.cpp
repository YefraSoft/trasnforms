// main.cpp
#include "MainWindow.h"
#include "WindowBuilder.h"
#include <iostream>

int main()
{
    // Crear ventana principal
    WindowConfig mainConfig(L"Transformaciones Geométricas - Principal", 500, 300, 100, 100);
    auto mainWindow = std::make_unique<MainWindow>(mainConfig);
    
    if (!mainWindow->Create())
    {
        std::wcout << L"Error: No se pudo crear la ventana principal" << std::endl;
        return -1;
    }
    
    mainWindow->SetRenderColor(0.1f, 0.1f, 0.2f);
    mainWindow->Show();
    
    std::wcout << L"Aplicación iniciada. Usa el botón para abrir la ventana de dibujo." << std::endl;
    
    // Loop de mensajes simple
    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Verificar si alguna ventana (principal o de dibujo) sigue activa
            if (!mainWindow->HasActiveWindows())
                break;

            Sleep(1);
        }
    }
    
    std::wcout << L"Aplicación finalizada." << std::endl;
    return 0;
}