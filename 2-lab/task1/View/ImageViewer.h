#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "IView.h"
#include "../Button.h"
#include "../Listeners/Document.h"

class ImageViewer :public IView
{
private:
    sf::RenderWindow &m_window;
    Document &m_document;

    sf::Font m_font;

    std::unique_ptr<Button> m_openButton;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

public:
    ImageViewer(sf::RenderWindow &window, Document &document)
        : m_window(window), m_document(document), m_sprite(m_texture)
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
            m_document.NotifyListeners("openFile");
        });
    }

    void ProcessEvents()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            }
            else if (auto key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    m_window.close();
                }
            }
            else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
                    if (m_openButton->Contains(mousePos))
                    {
                        m_openButton->OnClick();
                    }
                }
            }

        }
    }

    void SetImage(sf::Texture& texture, sf::IntRect& rect) override
    {
        m_texture = texture;

        m_sprite.setTextureRect(rect);
    }

    void Draw()
    {
        m_window.draw(m_sprite);
        m_openButton->DrawTo(m_window);
    }

    void Run()
    {
        while (m_window.isOpen())
        {
            ProcessEvents();
            Draw();
            m_window.display();
        }
    }
};
