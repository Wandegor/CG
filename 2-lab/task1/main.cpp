#include <SFML/Graphics.hpp>

#include "Button.h"
#include "Presenter/ImagePresenter.h"
#include "View/ImageViewer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1600, 1200}), "2.1");
    Document document;
    ImageModel model;

    ImageViewer viewer(window, document);
    ImagePresenter presenter(viewer, model, document);

    viewer.Run();
    return 0;
}
