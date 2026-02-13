#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>

class Button
{
private:
    sf::RectangleShape m_button;
    sf::Text m_text;
public:
    Button(
        sf::Font& font,
        std::string text,
        sf::Color textColor,

        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color backgroundColor)
    : m_text(font, text)
    {
        m_text.setFillColor(textColor);

        m_button.setPosition(position);
        m_button.setSize(size);
        m_button.setFillColor(backgroundColor);

        m_button.setOutlineThickness(2.0f);
        m_button.setOutlineColor(sf::Color::Black);
    }
    void DrawTo(sf::RenderWindow& window)
    {
        if (IsMouseOver(window))
        {
            m_button.setFillColor(sf::Color::Magenta);
        }
        window.draw(m_button);
        window.draw(m_text);
    }

    bool IsMouseOver(const sf::RenderWindow& window)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        sf::FloatRect buttonBounds = m_button.getGlobalBounds();

        return buttonBounds.contains(sf::Vector2f(mousePos));
    }
};



