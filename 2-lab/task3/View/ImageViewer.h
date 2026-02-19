#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "IView.h"
#include "../ViewComponents/Button.h"
#include "../Listeners/EventManager.h"
#include "../Listeners/EventType.h"
#include "../ViewComponents/Menu.h"

class ImageViewer :public IView
{
private:
    sf::RenderWindow &m_window;
    EventManager &m_manager;

    sf::Font m_font;

    std::optional<sf::Sprite> m_sprite;

public:
    ImageViewer(sf::RenderWindow &window, EventManager &document)
        : m_window(window), m_manager(document), m_sprite(std::nullopt)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }
    }

    void SetImage(sf::Texture &texture, sf::Vector2i screenPos) override
    {
        if (m_sprite.has_value())
        {
            m_sprite->setTexture(texture, true);
        } else
        {
            m_sprite.emplace(texture); // Будет вызван конструктор sf::Sprite(texture)
        }

        m_sprite->setPosition(sf::Vector2f(screenPos));
    }

    void MoveImage(sf::Vector2i delta) override
    {
        m_sprite->move(sf::Vector2f(delta));
    }

    void ProcessEvents()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            }
            // else if (auto key = event->getIf<sf::Event::KeyPressed>())
            // {
            //     if (key->code == sf::Keyboard::Key::Escape)
            //     {
            //         m_window.close();
            //     }
            // }
            // else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            // {
            //     if (mousePressed->button == sf::Mouse::Button::Left)
            //     {
            //         m_manager.NotifyListeners(EventType::MousePressed, event);
            //     }
            // }
            // else if (auto mouseMoved = event->getIf<sf::Event::MouseMoved>())
            // {
            //     m_manager.NotifyListeners(EventType::MouseMoved, event);
            // }
            // else if (auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            // {
            //     m_manager.NotifyListeners(EventType::MouseReleased, event);
            // }
        }
    }

    void Draw()
    {
        m_window.clear(sf::Color(200, 200, 200));

        float leftPanelWidth = 600.f;
        sf::RectangleShape leftPanel({leftPanelWidth, static_cast<float>(m_window.getSize().y)});
        leftPanel.setFillColor(sf::Color(180, 180, 180));
        m_window.draw(leftPanel);
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
