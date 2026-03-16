#pragma once
#pragma once
#include "Drawable.h"

class Rectangle : public Drawable
{
public:
    Rectangle(float w, float h)
    {
        std::vector<Point> vertices = {
            {-w / 2, -h / 2}, {w / 2, -h / 2}, {w / 2, h / 2},
            {-w / 2, -h / 2}, {w / 2, h / 2}, {-w / 2, h / 2}
        };
        Create(vertices);
    }
};
