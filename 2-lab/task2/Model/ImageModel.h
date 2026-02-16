#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class ImageModel
{
private:
    sf::Image m_image;
    sf::Texture m_texture;
    sf::Rect<int> m_rect;

public:
    ImageModel() = default;

    bool LoadFromFile(const std::string &filename)
    {
        if (!m_image.loadFromFile(filename))
        {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return false;
        }

        if (!m_texture.loadFromImage(m_image))
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

    bool SaveToFile(const std::string& filename) const
    {
        if (m_image.getSize().x == 0) return false;
        return m_image.saveToFile(filename);
    }

    void CreateNew(unsigned int width, unsigned int height, sf::Color color = sf::Color::Magenta)
    {
        m_image.resize({width, height}, color);
        if (!m_texture.loadFromImage(m_image)) return;
        m_rect.size.x = static_cast<int>(width);
        m_rect.size.y = static_cast<int>(height);
        m_rect.position = {200, 200};
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
