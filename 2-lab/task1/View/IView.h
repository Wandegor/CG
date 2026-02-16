#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetImage(sf::Texture& texture, sf::IntRect& rect) = 0;
    virtual void MoveImage(sf::Vector2f delta) = 0;
};