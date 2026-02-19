#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetImage(sf::Texture& texture, sf::Vector2i screenPos) = 0;
    virtual void StartTemporaryStroke(sf::Vector2i startPoint) = 0;
    virtual void AddTemporaryPoint(sf::Vector2i point) = 0;
    virtual std::vector<sf::Vector2i> FinishTemporaryStroke() = 0;
    virtual sf::Vector2f GetSpritePosition() const = 0;
};