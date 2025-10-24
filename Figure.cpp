// Figure.cpp
#include "Figure.h"

Figure::Figure(const std::string& figureName)
    : name(figureName), isComplete(false), figureColor(1.0f, 1.0f, 0.0f) // Default yellow
{
}

void Figure::AddPoint(const HomogenVector& point)
{
    points.push_back(point);
}

void Figure::AddPoint(float x, float y)
{
    points.emplace_back(x, y, 1);
}

void Figure::Clear()
{
    points.clear();
    isComplete = false;
    figureColor = Color(1.0f, 1.0f, 0.0f); // Reset to default yellow
}
