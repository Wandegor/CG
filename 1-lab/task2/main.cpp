#include "DragController.h"
#include "IShape.h"
#include "RocketScene.h"
#include "SceneRenderer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Starship rocket ");

    RocketScene scene;
    DragController dragController;

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

            dragController.HandleEvent(*event);
        }

        SceneRenderer::Draw(window, scene.GetShapes(), dragController.GetOffset());
        window.display();
    }

    return 0;
}
