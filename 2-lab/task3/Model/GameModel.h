#pragma once
#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

struct LibraryElement
{
    std::wstring name;
    sf::Texture texture;
};

struct FieldElement
{
    int libraryIndex;
    sf::Vector2f position;
};

class GameModel
{
private:
    std::vector<LibraryElement> m_library;
    std::vector<FieldElement> m_placedElements;

    std::map<std::pair<int,int>, std::vector<int>> m_recipes;
public:
    GameModel() = default;

    void InitLibrary()
    {
        auto createElement = [](const std::wstring &name, sf::Color color) -> LibraryElement
        {
            LibraryElement elem;
            elem.name = name;
            sf::Image img({80, 80}, color);
            if (!elem.texture.loadFromImage(img))
            {
                std::cerr << "Failed to load texture for " << std::string(name.begin(), name.end()) << std::endl;
            }
            return elem;
        };

        m_library.push_back(createElement(L"Земля", sf::Color::Blue));
        m_library.push_back(createElement(L"Огонь", sf::Color::Red));
        m_library.push_back(createElement(L"Вода", sf::Color::Cyan));
        m_library.push_back(createElement(L"Воздух", sf::Color::White));
        m_library.push_back(createElement(L"Лава", sf::Color::Yellow));
        m_library.push_back(createElement(L"Пар", sf::Color(200,200,200)));

        InitRecipes();
    }

    void InitRecipes()
    {
        m_recipes[{0,1}] = {4};
        m_recipes[{2,1}] = {5};
    }

    void AddFieldElement(int libIndex, sf::Vector2f pos) {
        m_placedElements.push_back({libIndex, pos});
    }

    void UpdateFieldElementPosition(int index, sf::Vector2f pos)
    {
        m_placedElements[index].position = pos;
    }

    std::vector<LibraryElement> &GetLibrary()
    {
        return m_library;
    }

    const std::vector<FieldElement> &GetFieldElements() const
    {
        return m_placedElements;
    }
};
