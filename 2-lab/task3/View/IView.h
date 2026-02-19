#pragma once
#include <SFML/Graphics.hpp>

class IView {
public:
    virtual ~IView() = default;

    virtual void SetLibrary(std::vector<LibraryElement> & library) = 0;
    virtual void SetFieldElements(const std::vector<PlacedElement> & elements) = 0;

    virtual int GetLibraryIndexAt(sf::Vector2i mousePos) const = 0;
    virtual sf::FloatRect GetFieldBounds() const = 0;

    // virtual void MoveImage(sf::Vector2i delta) = 0;
};