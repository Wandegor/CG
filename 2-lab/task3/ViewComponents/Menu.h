#pragma once

#include <utility>

#include "Button.h"

class Menu
{
private:
    Button m_headerButton;
    std::vector<Button> m_items;
    bool m_isOpen;

public:
    Menu(sf::Font &font, const std::string &title, sf::Vector2f position, sf::Vector2f size)
        : m_headerButton(font, title, position, size)
          , m_isOpen(false) {}

    void AddItem(const std::string &text, std::function<void()> onClick)
    {
        float x = m_headerButton.GetPosition().x + 10;
        float y = m_headerButton.GetPosition().y + m_headerButton.GetSize().y + 6.0f
                  + static_cast<float>(m_items.size()) * (m_headerButton.GetSize().y + 2.0f);

        Button item(m_headerButton.GetFont(),
                    text,
                    {x, y},
                    m_headerButton.GetSize(),
                    std::move(onClick));

        m_items.push_back(item);
    }

    void SetPosition(sf::Vector2f pos) {
        float dx = pos.x - m_headerButton.GetPosition().x;
        float dy = pos.y - m_headerButton.GetPosition().y;
        m_headerButton.SetPosition(pos);

        for (auto& item : m_items) {
            item.SetPosition(item.GetPosition() + sf::Vector2f(dx, dy));
        }
    }

    void Draw(sf::RenderWindow &window)
    {
        m_headerButton.DrawTo(window);
        if (m_isOpen)
        {
            for (auto &item: m_items)
            {
                item.DrawTo(window);
            }
        }
    }

    void HandleEvent(const sf::Event &event, sf::RenderWindow &window)
    {
        if (const auto *mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (m_headerButton.Contains(mousePos))
                {
                    m_isOpen = !m_isOpen;
                    return;
                }

                if (m_isOpen)
                {
                    for (auto &item : m_items)
                    {
                        if (item.Contains(mousePos))
                        {
                            item.OnClick();
                            m_isOpen = false;
                            return;
                        }
                    }
                    // Клик вне области
                    m_isOpen = false;
                }
            }
        }
    }
};
