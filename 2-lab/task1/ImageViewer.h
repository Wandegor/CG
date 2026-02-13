#pragma once
#include <SFML/Graphics.hpp>
#include "../portable-file-dialogs.h"
#include "Button.h"

class ImageViewer
{
private:
    sf::RenderWindow &m_window;
    sf::Font m_font;
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    std::unique_ptr<Button> m_openButton;

    void OpenFileDialog()
    {
        auto selection = pfd::open_file("Choose an image", ".",
                                        {"Image Files", "*.jpg *.jpeg"});
        if (!selection.result().empty())
        {
            std::string filename = selection.result()[0];
            LoadMyImage(filename);
        }
    }

    void LoadMyImage(const std::string &filename)
    {
        sf::Image image;
        if (!image.loadFromFile(filename))
        {
            std::cerr << "Failed to load image: " << filename << std::endl;
            return;
        }

        if (!m_texture.loadFromImage(image))
        {
            std::cerr << "Failed to create texture from image" << std::endl;
            return;
        }

        sf::IntRect rect({0, 0},
            {static_cast<int>(m_texture.getSize().x),
            static_cast<int>(m_texture.getSize().y)});

        m_sprite.setTextureRect(rect);
    }

public:
    ImageViewer(sf::RenderWindow &window)
        : m_window(window),
          m_sprite(m_texture)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        m_openButton = std::make_unique<Button>(
            m_font,
            "open",
            sf::Vector2f(100, 100),
            sf::Vector2f(200, 60)
        );

        m_openButton->SetOnClick([this]()
        {
            OpenFileDialog();
        });
    }

    void HandleEvent(const sf::Event &event)
    {
        if (const auto mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left)
            {
                if (m_openButton->Contains(sf::Mouse::getPosition(m_window)))
                {
                    m_openButton->OnClick();
                }
            }
        }
    }

    void Draw()
    {
        m_window.draw(m_sprite);
        m_openButton->DrawTo(m_window);
    }
};
