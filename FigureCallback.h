// FigureCallback.h - Callback system for asynchronous figure handling
#pragma once
#include "Figure.h"
#include <functional>
#include <memory>

using FigureCallback = std::function<void(std::shared_ptr<Figure>)>;

class FigureManager
{
private:
    static FigureCallback globalCallback;

public:
    static void SetGlobalCallback(FigureCallback callback);
    static void NotifyFigureComplete(std::shared_ptr<Figure> figure);
};
