#include<iostream>

#include "Canvas.h"
#include "Circle.h"

int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1200, 800}), "1.1");

    Canvas canvas(window);

    Circle circle(200,
                  Position(500, 500),
                  Color(200, 200, 1),
                  Color(234, 12, 21),
                  1);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                sf::View view(sf::FloatRect({0.f, 0.f}, sf::Vector2f(resized->size)));
                window.setView(view);
            }
        }

        canvas.Draw(circle);

        window.display();
    }

    return 0;
}
