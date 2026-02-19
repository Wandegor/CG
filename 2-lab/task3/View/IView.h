#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetLibrary(std::vector<ElementInfo> & library) = 0;

    // virtual void MoveImage(sf::Vector2i delta) = 0;
};