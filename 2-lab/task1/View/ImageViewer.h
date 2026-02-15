#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Button.h"
#include "../Listeners/Document.h"

class ImageViewer
{
private:
    sf::RenderWindow &m_window;
    sf::Font m_font;

    std::unique_ptr<Button> m_openButton;

    Document& m_document;

public:
    ImageViewer(sf::RenderWindow &window, Document& document)
        : m_window(window), m_document(document)
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

        // m_openButton->SetOnClick([this]()
        // {
        //     OpenFileDialog();
        // });
    }

    void Draw()
    {
        // m_window.draw(m_sprite);
        m_openButton->DrawTo(m_window);
    }

    void ProcessEvents()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            } else if (auto key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    m_window.close();
                }
            }

            // DAD ивенты
            else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                MouseData data{sf::Mouse::getPosition(m_window), mousePressed->button};
                m_document.NotifyListeners("mousePressed", &data);
            }
            else if (auto mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                sf::Vector2i pos = sf::Mouse::getPosition(m_window);
                m_document.NotifyListeners("mouseMoved", &pos);
            }
            else if (auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                m_document.NotifyListeners("mouseReleased", const_cast<sf::Mouse::Button *>(&mouseReleased->button));
            }
        }
    }

    void UpdateImage(sf::Sprite sprite)
    {

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

    struct MouseData
    {
        sf::Vector2i pos;
        sf::Mouse::Button button;
    };
};
