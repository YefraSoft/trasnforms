// Color.cpp - Implementation of Color utilities
#include "Color.h"

Color GetRainbowColor(float t)
{
    // t should be between 0.0 and 1.0
    t = t - (int)t; // Keep t in [0,1)

    if (t < 1.0f/6.0f)
        return Color(1.0f, t * 6.0f, 0.0f); // Red to Yellow
    else if (t < 2.0f/6.0f)
        return Color(1.0f - (t - 1.0f/6.0f) * 6.0f, 1.0f, 0.0f); // Yellow to Green
    else if (t < 3.0f/6.0f)
        return Color(0.0f, 1.0f, (t - 2.0f/6.0f) * 6.0f); // Green to Cyan
    else if (t < 4.0f/6.0f)
        return Color(0.0f, 1.0f - (t - 3.0f/6.0f) * 6.0f, 1.0f); // Cyan to Blue
    else if (t < 5.0f/6.0f)
        return Color((t - 4.0f/6.0f) * 6.0f, 0.0f, 1.0f); // Blue to Magenta
    else
        return Color(1.0f, 0.0f, 1.0f - (t - 5.0f/6.0f) * 6.0f); // Magenta to Red
}
