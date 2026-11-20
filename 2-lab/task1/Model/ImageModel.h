#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class ImageModel
{
private:
    sf::Image m_image;
    sf::Texture m_texture;
    sf::Vector2i m_picturePosition;

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
        m_picturePosition = {300, 300};

        return true;
    }

    void SetPosition(sf::Vector2i position)
    {
        m_picturePosition = position;
    }

    sf::Vector2i GetPicturePosition() const
    {
        return m_picturePosition;
    }

    sf::Texture &GetTexture()
    {
        return m_texture;
    }

    bool HasImage() const
    {
        return m_image.getSize().x > 0;
    }
};
