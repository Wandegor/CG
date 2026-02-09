#pragma once
#include <SFML/Graphics/CircleShape.hpp>

#include "../IShape.h"
#include "../Common/Position.h"
#include "../Common/Color.h"

class CircleAdapter : public IShape
{
    sf::CircleShape m_shape;

public:
    CircleAdapter(const float radius, const Color fillColor, const Position &position)
    {
        m_shape.setRadius(radius);
        m_shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b));
        m_shape.setPosition({position.m_x, position.m_y});
    }

    void Draw(sf::RenderWindow &window) const override
    {
        window.draw(m_shape);
    }
};
