#include <SFML/Graphics.hpp>

#include "Button.h"
#include "View/ImageViewer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1600, 1200}), "2.1");
    Document document;
    ImageViewer viewer(window, document);

    viewer.Run();

    return 0;
}
