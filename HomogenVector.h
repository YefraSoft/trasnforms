// HomogenVector.h - Homogeneous vector structure for geometric transformations
#pragma once

struct HomogenVector
{
    float x = 0.f; /**< x coordinate */
    float y = 0.f; /**< y coordinate */
    int w = 1;     /**< homogeneous component (formerly 'o') */
    
    HomogenVector() = default;
    HomogenVector(float x, float y, int w = 1) : x(x), y(y), w(w) {}
    
    // Convert from OpenGL coordinates (-1 to 1) to homogeneous coordinates
    static HomogenVector FromOpenGL(float glX, float glY)
    {
        return HomogenVector(glX, glY, 1);
    }
    
    // Convert to OpenGL coordinates for rendering
    void ToOpenGL(float& glX, float& glY) const
    {
        if (w != 0)
        {
            glX = x / w;
            glY = y / w;
        }
        else
        {
            glX = x;
            glY = y;
        }
    }
};
