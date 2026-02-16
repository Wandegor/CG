#pragma once

#include "../Listeners/IEventListener.h"
#include "../Model/ImageModel.h"

#include "../View/ImageViewer.h"
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
        m_manager.Subscribe(EventType::OpenFile, *this);
        m_manager.Subscribe(EventType::MousePressed, *this);
        m_manager.Subscribe(EventType::MouseMoved, *this);
        m_manager.Subscribe(EventType::MouseReleased, *this);
    }

    virtual ~ImagePresenter()
    {
        m_manager.Unsubscribe(EventType::OpenFile, *this);
        m_manager.Unsubscribe(EventType::MousePressed, *this);
        m_manager.Unsubscribe(EventType::MouseMoved, *this);
        m_manager.Unsubscribe(EventType::MouseReleased, *this);
    }

    void Update(EventType eventType, const sf::Event& event) override
    {
        switch (eventType)
        {
            case EventType::OpenFile:
                OnOpenFile();
            break;
            case EventType::MousePressed:
                OnMousePressed(event);
            break;
            case EventType::MouseMoved:
                OnMouseMoved(event);
            break;
            case EventType::MouseReleased:
                OnMouseReleased(event);
            break;
            default: ;
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
