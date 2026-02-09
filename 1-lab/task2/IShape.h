#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

class IShape
{
public:
    virtual ~IShape() = default;
    virtual void Draw(sf::RenderWindow &window) const = 0;
};
