#pragma once

#include "../Listeners/IEventListener.h"
#include "../Model/ImageModel.h"

#include "../View/ImageViewer.h"
#include "../MouseData.h"
#include <portable-file-dialogs.h>

class ImagePresenter : public IEventListener
{
private:
    IView &m_view;
    ImageModel &m_model;
    EventManager &m_manager;

    bool m_isDragging;
    sf::Vector2f m_lastMousePosition;

public:

    ImagePresenter(IView &view, ImageModel &model, EventManager &document)
            : m_view(view), m_model(model), m_manager(document), m_isDragging(false)
    {
        m_manager.Subscribe("openFile", *this);
        m_manager.Subscribe("mousePressed", *this);
        m_manager.Subscribe("mouseMoved", *this);
        m_manager.Subscribe("mouseReleased", *this);

    }

    virtual ~ImagePresenter()
    {
        m_manager.Unsubscribe("openFile", *this);
        m_manager.Unsubscribe("mousePressed", *this);
        m_manager.Unsubscribe("mouseMoved", *this);
        m_manager.Unsubscribe("mouseReleased", *this);
    }

    void Update(const std::string &eventType, const sf::Event& event) override
    {
        if (eventType == "openFile")
        {
            OnOpenFile();
        } else if (eventType == "mousePressed")
        {
            OnMousePressed(event);
        } else if (eventType == "mouseMoved")
        {
            OnMouseMoved(event);
        } else if (eventType == "mouseReleased")
        {
            OnMouseReleased(event);
        }
    }

private:
    void OnOpenFile()
    {
        auto selection = pfd::open_file("Choose an image", ".",
                                        {"Image Files", "*.jpg *.jpeg *.png *.bmp"});
        if (!selection.result().empty())
        {
            std::string filename = selection.result()[0];
            if (m_model.LoadFromFile(filename))
            {
                m_view.SetImage(m_model.GetTexture(), m_model.GetRect());
            }
        }
    }

    void OnMousePressed(const sf::Event& event)
    {
        auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        if (!mousePressed) return;
        if (mousePressed->button != sf::Mouse::Button::Left) return;

        if (m_model.GetRect().contains(mousePressed->position))
        {
            m_isDragging = true;
            m_lastMousePosition = {
                    static_cast<float>(mousePressed->position.x),
                    static_cast<float>(mousePressed->position.y)};

        }
    }

    void OnMouseMoved(const sf::Event& event)
    {
        auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
        if (!mouseMoved) return;

        if (!m_isDragging) return;

        sf::Vector2f currentPos(static_cast<float>(mouseMoved->position.x),
                            static_cast<float>(mouseMoved->position.y));
        sf::Vector2f delta = currentPos - m_lastMousePosition;

        m_model.Move(delta);
        m_view.MoveImage(delta);

        m_lastMousePosition = currentPos;
                
    }

    void OnMouseReleased(const sf::Event& event)
    {
        auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>();
        if (!mouseReleased) return;

        if (mouseReleased->button == sf::Mouse::Button::Left)
        {
            m_isDragging = false;
        }
    }
};
