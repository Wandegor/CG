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
    enum class DragInfo { None, Library, InGame } m_dragInfo;
    int m_dragLibraryIndex;

public:

    ImagePresenter(IView &view, GameModel &model, EventManager &document)
            : m_view(view), m_model(model), m_manager(document), m_isDragging(false)
    {
        m_manager.Subscribe(EventType::InitLibrary, *this);
        m_manager.Subscribe(EventType::MousePressed, *this);
        m_manager.Subscribe(EventType::MouseMoved, *this);
        m_manager.Subscribe(EventType::MouseReleased, *this);
    }

    virtual ~ImagePresenter()
    {
        m_manager.Unsubscribe(EventType::InitLibrary, *this);
        m_manager.Unsubscribe(EventType::MousePressed, *this);
        m_manager.Unsubscribe(EventType::MouseMoved, *this);
        m_manager.Unsubscribe(EventType::MouseReleased, *this);
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
                OnMousePressed(*event);
            break;
            case EventType::MouseMoved:
                OnMouseMoved(*event);
            break;
            case EventType::MouseReleased:
                OnMouseReleased(*event);
            break;
            default: ;
        }
    }

private:
    void OnMousePressed(const sf::Event& event)
    {
        auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        if (!mousePressed) return;
        if (mousePressed->button != sf::Mouse::Button::Left) return;

        int libIndex = m_view.GetLibraryIndexAt(mousePressed->position);
        // Клик по библиотеке
        if (libIndex != -1)
        {
            m_dragInfo = DragInfo::Library;
            m_dragLibraryIndex = libIndex;

            m_isDragging = true;
            m_lastMousePosition = mousePressed->position;

            const auto& lib = m_model.GetLibrary();
            const LibraryElement* info = &lib[libIndex];
            m_view.ShowDraggedElement(info, sf::Vector2f(mousePressed->position));
        }
        // Клики в другие места
    }

    void OnMouseMoved(const sf::Event& event)
    {
        auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
        if (!mouseMoved) return;
        if (!m_isDragging) return;

        sf::Vector2i currentPos(mouseMoved->position.x, mouseMoved->position.y);
        if (m_dragInfo == DragInfo::Library)
        {
            m_view.UpdateDraggedElement(sf::Vector2f(currentPos));
        }
    }

    void OnMouseReleased(const sf::Event& event)
    {
        auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>();
        if (!mouseReleased || !m_isDragging
            || mouseReleased->button != sf::Mouse::Button::Left)
            return;

        sf::FloatRect fieldBounds = m_view.GetFieldBounds();
        // мышь в правой области?
            // добавить на поле
        if (fieldBounds.contains(sf::Vector2f(mouseReleased->position)))
        {
            if (m_dragInfo == DragInfo::Library)
            {
                m_dragInfo = DragInfo::None;
                const auto& lib = m_model.GetLibrary();
                const LibraryElement* info = &lib[m_dragLibraryIndex];

                sf::Vector2f dropPos = sf::Vector2f(mouseReleased->position);

                m_model.AddPlacedElement(info, dropPos);
                m_view.SetFieldElements(m_model.GetPlacedElements());

            }
            // перемещение полевого элемента
        }

        m_view.HideDraggedElement();
        m_isDragging = false;
        m_dragInfo = DragInfo::None;
        // иначе вернуть на место(просто убрать призрак)
    }
};
