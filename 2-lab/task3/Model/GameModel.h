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
        sf::Texture texture;
        sf::Image image({60, 60}, sf::Color::Blue);

        if (!texture.loadFromImage(image))
        {
            std::cerr << "Failed to init texture elements" << std::endl;
        }

        ElementInfo earth;
        earth.name = L"Земля";
        earth.texture = texture;

        ElementInfo fire;
        fire.name = L"Огонь";
        fire.texture = texture;

        ElementInfo water;
        water.name = L"Вода";
        water.texture = texture;

        ElementInfo air;
        air.name = L"Воздух";
        air.texture = texture;

        m_library.push_back(earth);
        m_library.push_back(fire);
        m_library.push_back(water);
        m_library.push_back(air);
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
