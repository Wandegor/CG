#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class ImageModel
{
private:
    sf::Texture m_texture;
    sf::Rect<int> m_rect;

public:
    ImageModel() = default;

    bool LoadFromFile(const std::string &filename)
    {
        sf::Image image;
        if (!image.loadFromFile(filename))
        {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return false;
        }

        if (!m_texture.loadFromImage(image))
        {
            std::cerr << "Failed to create texture from image" << std::endl;
            return false;
        }

        sf::IntRect rect({0, 0},
                         {
                             static_cast<int>(m_texture.getSize().x),
                             static_cast<int>(m_texture.getSize().y)
                         });
        m_rect = rect;
        return true;
    }

    void Move(sf::Vector2f delta)
    {
        m_rect.position.x += static_cast<int>(delta.x);
        m_rect.position.y += static_cast<int>(delta.y);
    }

    sf::Rect<int> &GetRect()
    {
        return m_rect;
    }

    sf::Texture &GetTexture()
    {
        return m_texture;
    }
};
