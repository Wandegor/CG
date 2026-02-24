#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetImage(sf::Texture& texture, sf::Vector2i screenPos) = 0;
    virtual void MoveImage(sf::Vector2i delta) = 0;
    [[nodiscard]] virtual std::optional<sf::Vector2f> GetSpritePosition() const = 0;
};