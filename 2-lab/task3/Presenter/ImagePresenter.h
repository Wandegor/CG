#pragma once

#include "../Listeners/IEventListener.h"
#include "../Model/GameModel.h"

#include "../View/ImageViewer.h"

class ImagePresenter : public IEventListener
{
private:
    IView &m_view;
    GameModel &m_model;
    EventManager &m_manager;

    bool m_isDragging;
    sf::Vector2i m_lastMousePosition;

public:

    ImagePresenter(IView &view, GameModel &model, EventManager &document)
            : m_view(view), m_model(model), m_manager(document), m_isDragging(false)
    {
        m_manager.Subscribe(EventType::InitLibrary, *this);
        // m_manager.Subscribe(EventType::MousePressed, *this);
        // m_manager.Subscribe(EventType::MouseMoved, *this);
        // m_manager.Subscribe(EventType::MouseReleased, *this);
    }

    virtual ~ImagePresenter()
    {
        m_manager.Unsubscribe(EventType::InitLibrary, *this);
        // m_manager.Unsubscribe(EventType::MousePressed, *this);
        // m_manager.Unsubscribe(EventType::MouseMoved, *this);
        // m_manager.Unsubscribe(EventType::MouseReleased, *this);
    }

    void Update(EventType eventType, const std::optional<sf::Event> &event) override
    {
        switch (eventType)
        {
            case EventType::InitLibrary:
                m_model.InitLibrary();
                m_view.SetLibrary(m_model.GetLibrary());
            break;
            case EventType::MousePressed:
                // OnMousePressed(*event);
            break;
            case EventType::MouseMoved:
                // OnMouseMoved(*event);
            break;
            case EventType::MouseReleased:
                // OnMouseReleased(*event);
            break;
            default: ;
        }
    }

private:
    // void OnMousePressed(const sf::Event& event)
    // {
    //     auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
    //     if (!mousePressed) return;
    //     if (mousePressed->button != sf::Mouse::Button::Left) return;
    //     if (!m_model.HasImage()) return;
    //
    //     sf::Vector2i picturePos = m_model.GetPicturePosition();
    //     sf::Vector2i onImagePos(
    //         mousePressed->position.x - picturePos.x,
    //         mousePressed->position.y - picturePos.y
    //     );
    //
    //     // Проверка на попадание по картинке
    //     if (onImagePos.x >= 0 && onImagePos.y >= 0
    //         && onImagePos.x < m_model.GetTexture().getSize().x
    //         && onImagePos.y < m_model.GetTexture().getSize().y)
    //     {
    //         m_isDragging = true;
    //         m_lastMousePosition = mousePressed->position;
    //     }
    // }
    //
    // void OnMouseMoved(const sf::Event& event)
    // {
    //     auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
    //     if (!mouseMoved) return;
    //
    //     if (!m_isDragging) return;
    //
    //     sf::Vector2i currentPos(mouseMoved->position.x,
    //                         mouseMoved->position.y);
    //     sf::Vector2i delta = currentPos - m_lastMousePosition;
    //
    //     m_model.Move(delta);
    //     m_view.MoveImage(delta);
    //
    //     m_lastMousePosition = currentPos;
    //
    // }

    // void OnMouseReleased(const sf::Event& event)
    // {
    //     auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>();
    //     if (!mouseReleased) return;
    //
    //     if (mouseReleased->button == sf::Mouse::Button::Left)
    //     {
    //         m_isDragging = false;
    //     }
    // }
};
