#pragma once
#include "Drawable.h"

class LineStrip : public Drawable
{
public:
    explicit LineStrip(const std::vector<Point>& points)
    {
        primitiveType = GL_LINE_STRIP;
        Create(points);
    }
};
