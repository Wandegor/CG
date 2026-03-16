#pragma once
#include <cmath>
#include "Drawable.h"

class Circle : public Drawable
{
public:
    Circle(float r, float cx = 0, float cy = 0, int segments = 30)
    {
        std::vector<Point> vertices;
        for (int i = 0; i < segments; ++i)
        {
            float angle1 = 2.0f * M_PI * i / segments;
            float angle2 = 2.0f * M_PI * (i + 1) / segments;
            vertices.push_back({cx, cy});
            vertices.push_back({
                cx + r * std::cos(angle1),
                cy + r * std::sin(angle1)
            });
            vertices.push_back({
                cx + r * std::cos(angle2),
                cy + r * std::sin(angle2)
            });
        }
        primitiveType = GL_TRIANGLES;
        Create(vertices);
    }
};
