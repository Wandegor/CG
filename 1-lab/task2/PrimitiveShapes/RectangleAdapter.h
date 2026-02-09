#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../IShape.h"
#include "../Common/Position.h"
#include "../Common/Color.h"

class RectangleAdapter : public IShape
{
    sf::RectangleShape m_shape;

public:
    RectangleAdapter(const Position &size, const Color fillColor, const Position &position)
    {
        m_shape.setSize(sf::Vector2f(size.m_x, size.m_y));
        m_shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b));
        m_shape.setPosition({position.m_x, position.m_y});
    }

    void Draw(sf::RenderWindow &window) const override
    {
        window.draw(m_shape);
    }
};
