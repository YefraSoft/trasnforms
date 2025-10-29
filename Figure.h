// Figure.h - Figure class for storing and managing geometric figures
#pragma once
#include "HomogenVector.h"
#include "Color.h"
#include <vector>
#include <string>

class Figure
{
private:
    std::vector<HomogenVector> points;
    std::string name;
    bool isComplete;
    Color figureColor;

public:
    Figure(const std::string &figureName = "Figure");

    void AddPoint(const HomogenVector &point);
    void AddPoint(float x, float y);
    void SetComplete(bool complete) { isComplete = complete; }
    void SetColor(const Color &color) { figureColor = color; }

    std::vector<HomogenVector> &GetPoints() { return points; }
    std::string GetName() const { return name; }
    bool IsComplete() const { return isComplete; }
    size_t GetPointCount() const { return points.size(); }
    Color GetColor() const { return figureColor; }

    void Clear();
    void SetName(const std::string &newName) { name = newName; }
};

