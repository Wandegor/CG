#pragma once

#pragma once
#include <SFML/Graphics.hpp>
#include "Common/Position.h"

class DragController
{
private:
    Position m_offset;
    bool m_isDragging;
    Position m_dragStartPosition;

public:
    DragController() : m_offset{0, 0}, m_isDragging(false), m_dragStartPosition{0, 0} {}

    Position GetOffset() const { return m_offset; }

    void HandleEvent(const sf::Event &event)
    {
        if (const auto *mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left)
            {
                m_isDragging = true;
                m_dragStartPosition.m_x = static_cast<float>(mouseButtonPressed->position.x);
                m_dragStartPosition.m_y = static_cast<float>(mouseButtonPressed->position.y);
            }
        } else if (const auto mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseButtonReleased->button == sf::Mouse::Button::Left)
            {
                m_isDragging = false;
            }
        } else if (const auto mouseMove = event.getIf<sf::Event::MouseMoved>())
        {
            if (m_isDragging)
            {
                Position currentPosition{
                    static_cast<float>(mouseMove->position.x),
                    static_cast<float>(mouseMove->position.y)
                };

                Position delta{
                    currentPosition.m_x - m_dragStartPosition.m_x,
                    currentPosition.m_y - m_dragStartPosition.m_y
                };

                // Новое смещение
                m_offset.m_x -= delta.m_x;
                m_offset.m_y -= delta.m_y;
                m_dragStartPosition = currentPosition;
            }
        }
    }
};
