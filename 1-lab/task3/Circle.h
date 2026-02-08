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
        : m_radius(radius),
          m_position(position),
          m_fillColor(fillColor),
          m_outlineColor(outlineColor),
          m_outThickness(outThickness) {}

    int GetRadius() { return m_radius; }
    Position GetPosition() { return m_position; }
    Color GetFillColor() { return m_fillColor; }
    Color GetOutlineColor() { return m_outlineColor; }
    int GetOutThickness() { return m_outThickness; }
};
