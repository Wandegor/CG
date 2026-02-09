#pragma once

class Color
{
public:
    int r;
    int g;
    int b;

    explicit Color(int red = 0, int green = 0, int blue = 0)
        : r(red), g(green), b(blue) {}
};
