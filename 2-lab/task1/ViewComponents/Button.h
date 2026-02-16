#pragma once
#include <functional>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <utility>

class Button
{
private:
    sf::RectangleShape m_button;
    sf::Text m_text;

    std::function<void()> m_onClick;

    sf::Color m_normalColor;
    sf::Color m_hoverColor = sf::Color::Magenta;
public:
    Button(
        sf::Font& font,
        std::string text,
        sf::Vector2f position,
        sf::Vector2f size,

        sf::Color textColor = sf::Color(0, 0, 0),
        sf::Color bgNormalColor = sf::Color(225, 225, 225),
        sf::Color bgHoverColor  = sf::Color(255, 255, 200)
        )
    :
    m_text(font, text),
    m_normalColor(bgNormalColor),
    m_hoverColor(bgHoverColor)
    {
        m_text.setFillColor(textColor);

        sf::Vector2f buttonCenter = {
            position.x + size.x / 2.0f,
            position.y + size.y / 2.0f
        };

        m_text.setOrigin(m_text.getLocalBounds().getCenter());
        m_text.setPosition(buttonCenter);

        m_button.setPosition(position);
        m_button.setSize(size);
        m_button.setFillColor(m_normalColor);

        m_button.setOutlineThickness(5.0f);
        m_button.setOutlineColor(sf::Color(128, 128, 128));
    }
    void DrawTo(sf::RenderWindow& window)
    {
        m_button.setFillColor(IsMouseOver(window)
            ? m_hoverColor
            : m_normalColor);
        window.draw(m_button);
        window.draw(m_text);
    }

    bool IsMouseOver(const sf::RenderWindow& window)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        sf::FloatRect buttonBounds = m_button.getGlobalBounds();

        return buttonBounds.contains(sf::Vector2f(mousePos));
    }

    void SetOnClick(std::function<void()> callback)
    {
        m_onClick = std::move(callback);
    }

    void OnClick()
    {
        if (m_onClick)
        {
            m_onClick();
        }
    }

    bool Contains(sf::Vector2i position)
    {
        if (m_button.getGlobalBounds().contains(sf::Vector2f(position)))
        {
            return true;
        }
        return false;
    }
};



