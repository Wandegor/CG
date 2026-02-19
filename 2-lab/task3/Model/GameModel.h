#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

struct ElementInfo {
    std::string name;
    sf::Texture texture;
};

class GameModel
{
private:
    std::vector<ElementInfo> m_library;

public:
    GameModel() = default;

    void InitElements()
    {
        sf::Texture texture;
        sf::Image image({30, 30}, sf::Color::Blue);

        if (!texture.loadFromImage(image))
        {
            std::cerr << "Failed to init texture elements" << std::endl;
        }

        ElementInfo earth;
        earth.name = "Земля";
        earth.texture = texture;

        ElementInfo fire;
        fire.name = "Огонь";
        fire.texture = texture;

        ElementInfo water;
        water.name = "Вода";
        water.texture = texture;

        ElementInfo air;
        air.name = "Воздух";
        air.texture = texture;

        m_library.push_back(earth);
        m_library.push_back(fire);
        m_library.push_back(water);
        m_library.push_back(air);
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
    // sf::Texture &GetTexture()
    // {
    //     return m_texture;
    // }
    //
    // bool HasImage() const
    // {
    //     return m_image.getSize().x > 0;
    // }
    //
    // void UpdateTexture()
    // {
    //     m_texture.update(m_image);
    // }
};
