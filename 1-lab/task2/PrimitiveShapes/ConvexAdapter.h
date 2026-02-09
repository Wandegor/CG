#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

#include "../IShape.h"
#include "../Common/Position.h"
#include "../Common/Color.h"

class ConvexAdapter : public IShape
{
    sf::ConvexShape m_shape;

public:
    ConvexAdapter(const Position &p1, const Position &p2, const Position &p3, Color fillColor, Color outlineColor)
    {
        m_shape.setPointCount(3);

        m_shape.setPoint(0, sf::Vector2f(p1.m_x, p1.m_y));
        m_shape.setPoint(1, sf::Vector2f(p2.m_x, p2.m_y));
        m_shape.setPoint(2, sf::Vector2f(p3.m_x, p3.m_y));

        m_shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b));
        m_shape.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b));
    }

    void Draw(sf::RenderWindow &window) const override
    {
        window.draw(m_shape);
    }
};
