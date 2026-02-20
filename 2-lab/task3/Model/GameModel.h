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

    std::map<std::pair<int, int>, std::vector<int> > m_recipes;

public:
    GameModel() = default;

    void InitLibrary()
    {
        std::vector<std::pair<std::wstring, std::string>> elementData = {
            {L"Земля", "earth.png"},
            {L"Огонь", "fire.png"},
            {L"Вода", "water.png"},
            {L"Воздух", "air.png"},
            {L"Лава", "lava.png"},
            {L"Пар", "steam.png"}
        };

        for (const auto& [name, filename] : elementData)
        {
            LibraryElement elem;
            elem.name = name;

            std::string path = "D:/Projects/6-SEM/CG/2-lab/task3/Resources/" + filename;
            if (!elem.texture.loadFromFile(path))
            {
                std::cerr << "Failed to load texture " << path
                          << " for " << std::string(name.begin(), name.end()) << std::endl;
                sf::Image image = sf::Image({80, 80}, sf::Color::Blue);
                elem.texture.loadFromImage(image);
            }
            m_library.push_back(elem);
        }

        InitRecipes();
    }

    void InitRecipes()
    {
        m_recipes[{0, 1}] = {4};
        m_recipes[{2, 1}] = {5};
    }

    bool GetCombinationResult(int idxA, int idxB, std::vector<int>& outResults) const
    {
        auto it = m_recipes.find({idxA, idxB});
        if (it != m_recipes.end())
        {
            outResults = it->second;
            return true;
        }
        it = m_recipes.find({idxB, idxA});
        if (it != m_recipes.end())
        {
            outResults = it->second;
            return true;
        }
        return false;
    }

    int AddToLibraryIfNew(const LibraryElement& elem)
    {
        for (size_t i = 0; i < m_library.size(); ++i)
        {
            if (m_library[i].name == elem.name)
                return static_cast<int>(i);
        }
        m_library.push_back(elem);
        return static_cast<int>(m_library.size() - 1);
    }

    void AddFieldElement(int libIndex, sf::Vector2f pos)
    {
        m_placedElements.push_back({libIndex, pos});
    }

    void RemoveFieldElement(int index)
    {
        if (index >= 0 && index < m_placedElements.size())
            m_placedElements.erase(m_placedElements.begin() + index);
    }

    void UpdateFieldElementPosition(int index, sf::Vector2f pos)
    {
        if (index < 0 || index >= m_placedElements.size()) return;
        FieldElement elem = m_placedElements[index];
        elem.position = pos;
        m_placedElements.erase(m_placedElements.begin() + index);
        m_placedElements.push_back(elem);
    }

    std::vector<LibraryElement>& GetLibrary()
    {
        return m_library;
    }

    const std::vector<FieldElement>& GetFieldElements() const
    {
        return m_placedElements;
    }
};
