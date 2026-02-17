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

    bool SaveToFile(const std::string &filename) const
    {
        if (m_image.getSize().x == 0) return false;
        return m_image.saveToFile(filename);
    }

    void CreateNew(unsigned int width, unsigned int height, sf::Color color = sf::Color::Magenta)
    {
        m_image.resize({width, height}, color);
        if (!m_texture.loadFromImage(m_image)) return;
        m_picturePosition = {200, 200};
    }

    void DrawPoint(sf::Vector2i pos, sf::Color color)
    {
        if (pos.x >= 0 && pos.x < m_image.getSize().x &&
            pos.y >= 0 && pos.y < m_image.getSize().y)
        {
            m_image.setPixel({static_cast<unsigned>(pos.x), static_cast<unsigned>(pos.y)}, color);
        }
    }

    void DrawLine(sf::Vector2i from, sf::Vector2i to, sf::Color color)
    {
        int x1 = from.x, y1 = from.y;
        int x2 = to.x, y2 = to.y;
        int dx = abs(x2 - x1), dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            if (x1 >= 0 && x1 < (int) m_image.getSize().x &&
                y1 >= 0 && y1 < (int) m_image.getSize().y)
            {
                m_image.setPixel({static_cast<unsigned>(x1), static_cast<unsigned>(y1)}, color);
            }
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y1 += sy;
            }
        }
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

    void UpdateTexture()
    {
        m_texture.update(m_image);
    }
};
