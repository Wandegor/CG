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
        m_picturePosition = {100, 100};

        return true;
    }

    bool SaveToFile(const std::string &filename) const
    {
        if (m_image.getSize().x == 0) return false;
        return m_image.saveToFile(filename);
    }

    void CreateNew(unsigned int width, unsigned int height, sf::Color color = sf::Color::Magenta)
    {
        m_image.resize({width, height}, color);
        if (!m_texture.loadFromImage(m_image)) return;
        m_picturePosition = {200, 100};
    }

    sf::Vector2i GetPicturePosition() const
    {
        return m_picturePosition;
    }

    const sf::Texture &GetTexture() const
    {
        return m_texture;
    }

    bool HasImage() const
    {
        return m_image.getSize().x > 0;
    }

    void UpdateTexture(const sf::Texture &newTexture)
    {
        sf::Vector2u size = m_texture.getSize();
        if (size.x == 0 || size.y == 0) return;

        sf::RenderTexture canvas;
        if (!canvas.resize(size))
        {
            std::cerr << "Failed to create canvas" << std::endl;
            return;
        }

        canvas.clear(sf::Color::Transparent);
        canvas.draw(sf::Sprite(m_texture), sf::RenderStates(sf::BlendNone));
        canvas.draw(sf::Sprite(newTexture));
        canvas.display();

        m_image = canvas.getTexture().copyToImage();
        if(!m_texture.loadFromImage(m_image)){};
    }
};
