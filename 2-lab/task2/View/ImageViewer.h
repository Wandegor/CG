#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "IView.h"
#include "../../task1/Listeners/EventType.h"
#include "../ViewComponents/Button.h"
#include "../Listeners/EventManager.h"
#include "../ViewComponents/Menu.h"

class ImageViewer : public IView
{
private:
    sf::RenderWindow &m_window;
    EventManager &m_manager;

    sf::Font m_font;

    std::unique_ptr<Menu> m_fileMenu;

    std::optional<sf::Sprite> m_sprite;
    std::vector<sf::Vector2i> m_tempStrokePoints;
    sf::RenderTexture m_tempLayer;
    bool m_tempLayerExist = false;

public:
    ImageViewer(sf::RenderWindow &window, EventManager &document)
            : m_window(window), m_manager(document), m_sprite(std::nullopt)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        m_fileMenu = std::make_unique<Menu>(m_font, "File", sf::Vector2f(20, 20), sf::Vector2f(100, 40));
        m_fileMenu->AddItem("New", [this]()
        {
            m_manager.NotifyListeners(EventType::NewFile);
        });
        m_fileMenu->AddItem("Open", [this]()
        {
            m_manager.NotifyListeners(EventType::OpenFile);
        });
        m_fileMenu->AddItem("Save", [this]()
        {
            m_manager.NotifyListeners(EventType::SaveFile);
        });
    }

    void SetImage(const sf::Texture &texture, sf::Vector2i screenPos) override
    {
        if (m_sprite.has_value())
        {
            m_sprite->setTexture(texture, true);
        } else
        {
            m_sprite.emplace(texture); // Будет вызван конструктор sf::Sprite(texture)
        }

        m_sprite->setPosition(sf::Vector2f(screenPos));

        sf::Vector2u texSize = texture.getSize();
        if (texSize.x > 0 && texSize.y > 0)
        {
            if (!m_tempLayer.resize(texSize))
                std::cerr << "Failed to resize temporary layer" << std::endl;
            else
            {
                m_tempLayer.clear(sf::Color::Transparent);
                m_tempLayer.display();
                m_tempLayerExist = false;
            }
        }
    }

    void StartTemporaryStroke(sf::Vector2i startPoint) override
    {
        if (!m_sprite.has_value()) return;

        sf::Vector2u texSize = m_sprite->getTexture().getSize();
        if (texSize.x == 0 || texSize.y == 0) return;

        if (m_tempLayer.getSize() != texSize)
        {
            if (!m_tempLayer.resize(texSize))
            {
                std::cerr << "Failed to resize temporary layer" << std::endl;
                return;
            }
        }

        m_tempLayerExist = true;
        m_tempLayer.clear(sf::Color::Transparent);

        sf::Vertex point(sf::Vector2f(startPoint), sf::Color::Black);
        m_tempLayer.draw(&point, 1, sf::PrimitiveType::Points);
        m_tempLayer.display();
    }

    void AddTemporaryPoint(sf::Vector2i from, sf::Vector2i to) override
    {
        if (!m_tempLayerExist) return;
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(from), sf::Color::Black),
                sf::Vertex(sf::Vector2f(to), sf::Color::Black)
        };
        m_tempLayer.draw(line, 2, sf::PrimitiveType::Lines);
        m_tempLayer.display();
    }

    const sf::Texture &FinishTemporaryStroke() override
    {
        return m_tempLayer.getTexture();
    }

    void DrawPoint(sf::Vector2i pos) override
    {
        if (!m_tempLayerExist) return;
        sf::Vertex point(sf::Vector2f(pos), sf::Color::Black);
        m_tempLayer.draw(&point, 1, sf::PrimitiveType::Points);
        m_tempLayer.display();
    }

    void ClearTemporary() override
    {
        m_tempLayerExist = false;
    }

    sf::Vector2f GetSpritePosition() const override
    {
        return m_sprite.has_value()
               ? m_sprite->getPosition()
               : sf::Vector2f(0, 0);
    }

    void ProcessEvents()
    {
        while (const std::optional<sf::Event> event = m_window.pollEvent())
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
            } else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    m_manager.NotifyListeners(EventType::MousePressed, event);
                }
            } else if (event->getIf<sf::Event::MouseMoved>())
            {
                m_manager.NotifyListeners(EventType::MouseMoved, event);
            } else if (event->getIf<sf::Event::MouseButtonReleased>())
            {
                m_manager.NotifyListeners(EventType::MouseReleased, event);
            } else if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
                m_window.setView(sf::View(visibleArea));
            }

            m_fileMenu->HandleEvent(*event, m_window);
        }
    }

    void Draw()
    {
        m_window.clear(sf::Color(200, 200, 200));
        if (m_sprite.has_value())
            m_window.draw(m_sprite.value());

        if (m_tempLayerExist)
        {
            sf::Sprite tempSprite(m_tempLayer.getTexture());
            tempSprite.setPosition(m_sprite->getPosition());
            m_window.draw(tempSprite);
        }
        m_fileMenu->Draw(m_window);
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
