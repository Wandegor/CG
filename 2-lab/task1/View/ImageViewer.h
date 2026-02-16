#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "IView.h"
#include "../Button.h"
#include "../Listeners/EventManager.h"
#include "../MouseData.h"

class ImageViewer :public IView
{
private:
    sf::RenderWindow &m_window;
    EventManager &m_manager;

    sf::Font m_font;

    std::unique_ptr<Button> m_openButton;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

public:
    ImageViewer(sf::RenderWindow &window, EventManager &document)
        : m_window(window), m_manager(document), m_sprite(m_texture)
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
            m_manager.NotifyListeners("openFile");
        });
    }

    void SetImage(sf::Texture& texture, sf::IntRect& rect) override
    {
        m_texture = texture;
        m_sprite.setTextureRect(rect);
    }

    void MoveImage(sf::Vector2f delta) override
    {
        m_sprite.move(delta);
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
                    else
                    {
                        MouseData data{sf::Mouse::getPosition(m_window), mousePressed->button};
                        m_manager.NotifyListeners("mousePressed", &data);
                    }
                }
            }

            else if (auto mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                sf::Vector2i pos = sf::Mouse::getPosition(m_window);
                m_manager.NotifyListeners("mouseMoved", &pos);
            }
            else if (auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                m_manager.NotifyListeners("mouseReleased", const_cast<sf::Mouse::Button *>(&mouseReleased->button));
            }
        }
    }

    void Draw()
    {
        m_window.clear(sf::Color(200, 200, 200));
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
