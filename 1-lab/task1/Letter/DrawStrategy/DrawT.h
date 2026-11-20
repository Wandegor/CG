#pragma once
#include "IDrawStrategy.h"
#include "../Color.h"
#include "../Position.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"

class DrawT: public IDrawStrategy
{
public:
    void Draw(sf::RenderWindow& window, Color color, Position position) override
    {
        auto shapeColor = sf::Color(color.r, color.g, color.b);

        sf::RectangleShape top(sf::Vector2f(200, 50));
        top.setFillColor(shapeColor);
        top.setPosition({position.m_x, position.m_y});

        sf::RectangleShape vertical(sf::Vector2f(50, 200));
        vertical.setFillColor(shapeColor);
        vertical.setPosition({position.m_x + 75, position.m_y + 50});

        window.draw(top);
        window.draw(vertical);
    }
};
