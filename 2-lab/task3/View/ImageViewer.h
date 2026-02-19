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

    std::vector<sf::Sprite> m_librarySprites;
    std::vector<sf::Text> m_libraryTexts;

public:
    ImageViewer(sf::RenderWindow &window, EventManager &document)
        : m_window(window), m_manager(document)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }
    }

    void SetLibrary(std::vector<ElementInfo> & library) override
    {
        m_librarySprites.clear();
        m_libraryTexts.clear();
        const float startY = 20.f;
        const float padding = 10.f;
        const float iconSize = 64.f;

        for (size_t i = 0; i < library.size(); ++i)
        {
            const auto& elem = library[i];

            sf::Sprite sprite(elem.texture);
            // Позиция: слева, по вертикали с отступом
            sprite.setPosition({padding, startY + i * (iconSize + padding)});
            m_librarySprites.push_back(sprite);

            sf::Text text(m_font, elem.name);
            text.setFillColor(sf::Color::Black);
            // Размещаем справа от иконки
            float textX = iconSize + 2 * padding;
            float textY = startY + i * (iconSize + padding) + iconSize/2 - text.getLocalBounds().size.y/2;
            text.setPosition({textX, textY});
            m_libraryTexts.push_back(text);
        }
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

        for (const auto& sprite : m_librarySprites)
            m_window.draw(sprite);
        for (const auto& text : m_libraryTexts)
            m_window.draw(text);
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
