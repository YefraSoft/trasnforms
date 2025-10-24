// Figure.cpp
#include "Figure.h"

Figure::Figure(const std::string& figureName)
    : name(figureName), isComplete(false)
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
}
