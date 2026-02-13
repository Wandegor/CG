#include <iostream>
#include <SFML/Graphics.hpp>

#include "Button.h"
#include "ImageViewer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1600, 1200}), "2.1");

    ImageViewer viewer(window);

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
            else
            {
                viewer.HandleEvent(*event);
            }
        }
        window.clear(sf::Color(192, 192, 192));

        viewer.Draw();
        window.display();
    }

    return 0;
}
