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
    float m_leftPanelWidth;


public:
    ImageViewer(sf::RenderWindow &window, EventManager &document)
        : m_window(window), m_manager(document)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        m_leftPanelWidth = m_window.getSize().x * 0.4f;
    }

    void SetLibrary(std::vector<ElementInfo>& library) override
    {
        m_librarySprites.clear();
        m_libraryTexts.clear();

        const float elemSize = library[0].texture.getSize().x;
        const int columns = 4;

        const float spacing = m_leftPanelWidth / columns - elemSize;

        const float textOffsetY = 3.f;
        const float cellWidth = elemSize + spacing;
        const float cellHeight = elemSize + spacing + 30.f;

        for (size_t i = 0; i < library.size(); ++i)
        {
            const auto& elem = library[i];
            int col = static_cast<int>(i % columns);
            int row = static_cast<int>(i / columns);

            float x = spacing/2 + col * cellWidth;
            float y = 20 + row * cellHeight;

            sf::Sprite sprite(elem.texture);
            sprite.setPosition({x, y});
            m_librarySprites.push_back(sprite);

            sf::Text text(m_font, elem.name);
            text.setCharacterSize(16);
            text.setFillColor(sf::Color::Black);

            sf::FloatRect textBounds = text.getLocalBounds();
            float textX = x + (elemSize - textBounds.size.x) / 2.f;
            float textY = y + elemSize + textOffsetY;
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

        sf::RectangleShape leftPanel({m_leftPanelWidth, static_cast<float>(m_window.getSize().y)});
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
