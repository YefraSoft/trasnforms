// FigureCallback.cpp
#include "FigureCallback.h"

FigureCallback FigureManager::globalCallback = nullptr;

void FigureManager::SetGlobalCallback(FigureCallback callback)
{
    globalCallback = callback;
}

void FigureManager::NotifyFigureComplete(std::shared_ptr<Figure> figure)
{
    if (globalCallback)
    {
        globalCallback(figure);
    }
}
