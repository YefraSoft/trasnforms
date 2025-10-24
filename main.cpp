// main.cpp
#include "WindowBuilder.h"
#include <iostream>

int main()
{
    // Crear el builder
    WindowBuilder builder;

    // Crear primera ventana
    auto window1 = builder.Build(L"Ventana 1 - OpenGL", 800, 600);
    if (window1)
    {
        window1->SetRenderColor(0.2f, 0.3f, 0.4f);
        window1->Show();
    }

    // Crear segunda ventana con configuración personalizada
    WindowConfig config2(L"Ventana 2 - OpenGL", 640, 480, 100, 100);
    auto window2 = builder.Build(config2);
    if (window2)
    {
        window2->SetRenderColor(0.4f, 0.2f, 0.3f);
        window2->Show();
    }

    // Crear tercera ventana
    auto window3 = builder.Build(L"Ventana 3 - OpenGL", 500, 400);
    if (window3)
    {
        window3->SetRenderColor(0.3f, 0.4f, 0.2f);
        window3->Show();
    }

    // Obtener el comunicador para enviar mensajes entre ventanas
    auto communicator = builder.GetCommunicator();

    std::wcout << L"Ventanas creadas: " << builder.GetWindows().size() << std::endl;
    std::wcout << L"Presiona cualquier tecla para cambiar los titulos..." << std::endl;

    // Simular comunicación después de un tiempo
    Sleep(2000);

    // Ejemplo de broadcast: todas las ventanas recibirán este mensaje
    communicator->BroadcastMessage(L"Test Message");

    if (window1 && window1->IsActive())
    {
        window1->SetTitle(L"Ventana 1 - Actualizada");
    }

    // Enviar mensaje broadcast
    communicator->BroadcastMessage(L"System Ready");

    std::wcout << L"Loop de mensajes iniciado. Cierra todas las ventanas para salir." << std::endl;

    // Iniciar el loop de mensajes
    builder.MessageLoop();

    std::wcout << L"Aplicacion finalizada." << std::endl;

    return 0;
}