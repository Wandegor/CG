#pragma once
#include <SFML/Graphics.hpp>

class IView
{
public:
    virtual ~IView() = default;

    virtual void SetLibrary(const std::vector<LibraryElement>& library, const std::vector<int>& unlockedIndices) = 0;
    virtual void SetFieldElements(const std::vector<FieldElement>& elements,
                                  const std::vector<LibraryElement>& library) = 0;

    virtual void UpdateFieldElementPosition(int index, sf::Vector2f newPos) = 0;

    virtual void ShowDraggedElement(const LibraryElement* element, sf::Vector2f screenPos, sf::Vector2f offset) = 0;
    virtual void UpdateDraggedElement(sf::Vector2f screenPos) = 0;
    virtual void HideDraggedElement() = 0;
    virtual void HideFieldElement(int index) = 0;

    [[nodiscard]] virtual std::wstring GetLibraryElementNameAt(sf::Vector2i mousePos) const = 0;
    [[nodiscard]] virtual sf::Vector2f GetLibraryElementPosition(int index) const = 0;
    [[nodiscard]] virtual int GetLibraryIndexAt(sf::Vector2i mousePos) const = 0;
    [[nodiscard]] virtual int GetFieldIndexAt(sf::Vector2i mousePos) const = 0;

    [[nodiscard]] virtual sf::FloatRect GetDelElemBounds() const = 0;
    [[nodiscard]] virtual sf::FloatRect GetFieldBounds() const = 0;
    [[nodiscard]] virtual float GetIconSize() const = 0;
};
