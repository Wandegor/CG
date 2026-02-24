#include <SFML/Graphics.hpp>

#include "ViewComponents/Button.h"
#include "Listeners/EventManager.h"
#include "Presenter/ImagePresenter.h"
#include "View/ImageViewer.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "2.3");
    EventManager eventManager;
    GameModel model;

    ImageViewer viewer(window, eventManager);
    ImagePresenter presenter(viewer, model, eventManager);

    eventManager.NotifyListeners(EventType::InitLibrary);

    viewer.Run();
    return 0;
}
