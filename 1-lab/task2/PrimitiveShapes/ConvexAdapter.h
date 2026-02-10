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

        m_shape.setPoint(0, sf::Vector2f(p1.x, p1.y));
        m_shape.setPoint(1, sf::Vector2f(p2.x, p2.y));
        m_shape.setPoint(2, sf::Vector2f(p3.x, p3.y));

        m_shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b));
        m_shape.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b));
    }

    void Draw(sf::RenderTarget &window) const override
    {
        window.draw(m_shape);
    }
};
