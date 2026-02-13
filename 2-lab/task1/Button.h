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
        sf::String text,
        sf::Color textColor,

        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color backgroundColor)
    {
        m_text.setFont(font);
        m_text.setString(text);
        m_text.setFillColor(textColor);

        m_button.setPosition(position);
        m_button.setSize(size);
        m_button.setFillColor(backgroundColor);
    }

    void DrawTo(sf::RenderWindow& window)
    {
        window.draw(m_button);
        window.draw(m_text);
    }

    bool IsMouseOver(sf::RenderWindow& window)
    {

    }
};



