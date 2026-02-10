#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

class IShape
{
public:
    virtual ~IShape() = default;
    virtual void Draw(sf::RenderTarget &window) const = 0;
};
