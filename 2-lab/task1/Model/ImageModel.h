#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class ImageModel
{
private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

public:
    ImageModel(): m_sprite(m_texture)
    {}

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

        sf::Rect rect(sf::Vector2i(300, 300),
                               {
                                   static_cast<int>(m_texture.getSize().x),
                                   static_cast<int>(m_texture.getSize().y)
                               });

        m_sprite.setTextureRect(rect);
        return true;
    }

    void SetPosition(float x, float y)
    {
        m_sprite.setPosition({x, y});
    }

    void Move(const sf::Vector2f delta)
    {
        m_sprite.move(delta);
    }

    sf::Vector2f GetPosition() const
    {
        return m_sprite.getPosition();
    }

    sf::Sprite GetSprite()
    {
        return m_sprite;
    }
};
