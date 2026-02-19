#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

struct ElementInfo {
    std::wstring name;
    sf::Texture texture;
};

class GameModel
{
private:
    std::vector<ElementInfo> m_library;

public:
    GameModel() = default;

    void InitLibrary()
    {
        auto createElement = [](const std::wstring& name, sf::Color color) -> ElementInfo {
            ElementInfo elem;
            elem.name = name;
            sf::Image img({60, 60}, color);
            if (!elem.texture.loadFromImage(img)) {
                std::cerr << "Failed to load texture for " << std::string(name.begin(), name.end()) << std::endl;
            }
            return elem;
        };

        m_library.push_back(createElement(L"Земля", sf::Color::Blue));
        m_library.push_back(createElement(L"Огонь", sf::Color::Red));
        m_library.push_back(createElement(L"Вода", sf::Color::Cyan));
        m_library.push_back(createElement(L"Воздух", sf::Color::White));
        m_library.push_back(createElement(L"Воздух1231", sf::Color::Green));
        m_library.push_back(createElement(L"СнегВРоссии", sf::Color::Yellow));
    }

    std::vector<ElementInfo> &GetLibrary()
    {
        return m_library;
    }

    // void Move(sf::Vector2i delta)
    // {
    //     m_picturePosition.x += delta.x;
    //     m_picturePosition.y += delta.y;
    // }
    //
    // sf::Vector2i GetPicturePosition() const
    // {
    //     return m_picturePosition;
    // }
    //
    //
    //
};
