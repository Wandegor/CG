#include <iostream>
#include <SFML/Graphics.hpp>

#include "Button.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1600, 1200}), "2.1");

    sf::Font arial;

    if (arial.openFromFile("ArialRegular.ttf"))
    {
        std::cout << "OK" << std::endl;
    }

    Button myButton = Button(
        arial,
        std::string("my but"),
        sf::Vector2f(100, 100),
        sf::Vector2f(200, 60));

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }
        window.clear(sf::Color(192, 192, 192));

        myButton.DrawTo(window);
        window.display();
    }

    return 0;
}
