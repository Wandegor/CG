#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetImage(const sf::Texture& texture, sf::Vector2i screenPos) = 0;

    virtual void StartTemporaryStroke(sf::Vector2i startPoint) = 0;
    virtual void AddTemporaryPoint(sf::Vector2i from, sf::Vector2i to) = 0;
    virtual const sf::Texture& FinishTemporaryStroke() = 0;

    [[nodiscard]] virtual sf::Vector2f GetSpritePosition() const = 0;
};