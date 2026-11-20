#include <SFML/Graphics.hpp>

#include "Letter/Letter.h"
#include "Letter/DrawStrategy/DrawA.h"
#include "Letter/DrawStrategy/DrawE.h"
#include "Letter/DrawStrategy/DrawT.h"
#include "Letter/JumpStrategy/UniformJump.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "1.1");

    Letter T = Letter(
        Color(255, 0, 0),
        Position(100, 300),
        std::make_unique<DrawT>(),
        std::make_unique<UniformJump>(550.0f, 980.0f)
    );

    Letter E = Letter(
        Color(0, 255, 0),
        Position(350, 300),
        std::make_unique<DrawE>(),
        std::make_unique<UniformJump>(650.0f, 980.0f)
    );

    Letter A = Letter(
        Color(0, 0, 255),
        Position(550, 300),
        std::make_unique<DrawA>(),
        std::make_unique<UniformJump>(750.0f, 980.0f)
    );

    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        // логически правильней
        T.Update(deltaTime);
        E.Update(deltaTime);
        A.Update(deltaTime);

        window.clear(sf::Color::Black);

        T.Draw(window);
        E.Draw(window);
        A.Draw(window);

        window.display();
    }

    return 0;
}
