#pragma once
#include "IDrawStrategy.h"
#include "../Color.h"
#include "../Position.h"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

class DrawA : public IDrawStrategy
{
public:
    void Draw(sf::RenderTarget &window, Color color, Position position) override
    {
        auto shapeColor = sf::Color(color.r, color.g, color.b);

        const float width = 200.0f;
        const float height = 250.0f;

        sf::ConvexShape outerTriangle;
        outerTriangle.setPointCount(3);
        outerTriangle.setPoint(0, sf::Vector2f(0 + 50, height));
        outerTriangle.setPoint(1, sf::Vector2f(width / 2 + 50, 0));
        outerTriangle.setPoint(2, sf::Vector2f(width + 50, height));
        outerTriangle.setFillColor(shapeColor);
        outerTriangle.setPosition({position.x, position.y});

        sf::ConvexShape innerTriangle;
        innerTriangle.setPointCount(3);
        innerTriangle.setPoint(0, sf::Vector2f(0+100, height));
        innerTriangle.setPoint(1, sf::Vector2f(width / 2 + 50, 0+100));
        innerTriangle.setPoint(2, sf::Vector2f(width, height));

        innerTriangle.setFillColor(sf::Color::Black);
        innerTriangle.setPosition({position.x, position.y});

        sf::RectangleShape crossbar(sf::Vector2f(50, 100));
        crossbar.setFillColor(shapeColor);
        crossbar.setRotation(sf::degrees(90));
        crossbar.setPosition({position.x + width, position.y + height * 0.65f});


        window.draw(outerTriangle);
        window.draw(innerTriangle);
        window.draw(crossbar);
    }
};
