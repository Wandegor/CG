#pragma once
#include "IDrawStrategy.h"
#include "../Color.h"
#include "../Position.h"
#include "SFML/Graphics/RectangleShape.hpp"

class DrawE: public IDrawStrategy
{
public:
    void Draw(sf::RenderWindow& window, Color color, Position position) override
    {
        auto shapeColor = sf::Color(color.r, color.g, color.b);

        const float width = 200.0f;
        const float height = 250.0f;
        const float thickness = 50.0f;

        sf::RectangleShape vertical(sf::Vector2f(thickness, height));
        vertical.setFillColor(shapeColor);
        vertical.setPosition({position.m_x, position.m_y});

        sf::RectangleShape top(sf::Vector2f(width - thickness, thickness));
        top.setFillColor(shapeColor);
        top.setPosition({position.m_x + thickness, position.m_y});

        sf::RectangleShape middle(sf::Vector2f(width - thickness, thickness));
        middle.setFillColor(shapeColor);
        middle.setPosition({position.m_x + thickness, position.m_y + height / 2 - thickness / 2});

        sf::RectangleShape bottom(sf::Vector2f(width - thickness, thickness));
        bottom.setFillColor(shapeColor);
        bottom.setPosition({position.m_x + thickness, position.m_y + height - thickness});

        window.draw(vertical);
        window.draw(top);
        window.draw(middle);
        window.draw(bottom);
    }
};