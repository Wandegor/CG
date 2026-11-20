#include <SFML/Graphics.hpp>

#include "ViewComponents/Button.h"
#include "Listeners/EventManager.h"
#include "Presenter/ImagePresenter.h"
#include "View/ImageViewer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1200, 1000}), "2.1");
    EventManager document;
    ImageModel model;

    ImageViewer viewer(window, document);
    ImagePresenter presenter(viewer, model, document);

    viewer.Run();
    return 0;
}
