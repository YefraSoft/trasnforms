// Figure.h - Figure class for storing and managing geometric figures
#pragma once
#include "HomogenVector.h"
#include <vector>
#include <string>

class Figure
{
private:
    std::vector<HomogenVector> points;
    std::string name;
    bool isComplete;

public:
    Figure(const std::string& figureName = "Figure");
    
    void AddPoint(const HomogenVector& point);
    void AddPoint(float x, float y);
    void SetComplete(bool complete) { isComplete = complete; }
    
    const std::vector<HomogenVector>& GetPoints() const { return points; }
    std::string GetName() const { return name; }
    bool IsComplete() const { return isComplete; }
    size_t GetPointCount() const { return points.size(); }
    
    void Clear();
    void SetName(const std::string& newName) { name = newName; }
};
