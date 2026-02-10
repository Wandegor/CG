#pragma once
#include "SFML/Graphics/RenderWindow.hpp"

class IDrawStrategy
{
public:
    virtual ~IDrawStrategy() = default;
    virtual void Draw(sf::RenderTarget &window, Color color, Position position) = 0;
};
