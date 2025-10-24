// Color.h - Color constants and utilities for the application
#pragma once

struct Color
{
    float r, g, b;

    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(float red, float green, float blue) : r(red), g(green), b(blue) {}

    // Convert to OpenGL color array
    void ToOpenGL(float* colorArray) const
    {
        colorArray[0] = r;
        colorArray[1] = g;
        colorArray[2] = b;
    }
};

// Color palette constants
const Color RED = {1.0f, 0.0f, 0.0f};
const Color GREEN = {0.0f, 1.0f, 0.0f};
const Color BLUE = {0.0f, 0.0f, 1.0f};
const Color WHITE = {1.0f, 1.0f, 1.0f};
const Color BLACK = {0.0f, 0.0f, 0.0f};
const Color YELLOW = {1.0f, 1.0f, 0.0f};
const Color CYAN = {0.0f, 1.0f, 1.0f};
const Color MAGENTA = {1.0f, 0.0f, 1.0f};
const Color GRAY = {0.5f, 0.5f, 0.5f};
const Color ORANGE = {1.0f, 0.5f, 0.0f};
const Color LIGHT_BLUE = {0.53f, 0.81f, 0.92f};
const Color PINK = {1.0f, 0.75f, 0.8f};
const Color DARK_GREEN = {0.0f, 0.5f, 0.0f};
const Color BROWN = {0.65f, 0.16f, 0.16f};
const Color PURPLE = {0.5f, 0.0f, 0.5f};
const Color GOLD = {1.0f, 0.84f, 0.0f};
const Color SKY_BLUE = {0.4f, 0.7f, 1.0f};
const Color LIME = {0.75f, 1.0f, 0.0f};
const Color DARK_RED = {0.55f, 0.0f, 0.0f};

// Rainbow color gradient function declaration
Color GetRainbowColor(float t);
