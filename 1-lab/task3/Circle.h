#pragma once

#include "../task1/Letter/Color.h"
#include "../task1/Letter/Position.h"

class Circle
{
    float m_radius;
    Position m_position;
    Color m_fillColor;
    Color m_outlineColor;
    int m_outThickness;

public:
    Circle(float radius, Position position, Color fillColor, Color outlineColor, int outThickness)
        :
          m_position(position),
          m_fillColor(fillColor),
          m_outlineColor(outlineColor),
          m_outThickness(outThickness)
    {
        if (radius <= 0)
        {
            std::cout << "radius must be greater than 0" << std::endl;
            exit(1);
        }
        m_radius = radius;
    }

    // методы, которые не изменяют состояние объекта должны быть константными
    const int GetRadius() { return m_radius; }
    const Position GetPosition() { return m_position; }
    const Color GetFillColor() { return m_fillColor; }
    const Color GetOutlineColor() { return m_outlineColor; }
    const int GetOutThickness() { return m_outThickness; }
};
