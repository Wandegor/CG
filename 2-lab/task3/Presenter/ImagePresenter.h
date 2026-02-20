#pragma once

#include "../Listeners/IEventListener.h"
#include "../Model/GameModel.h"

#include "../View/ImageViewer.h"

class ImagePresenter : public IEventListener
{
private:
    IView& m_view;
    GameModel& m_model;
    EventManager& m_manager;

    bool m_isDragging;
    sf::Vector2i m_lastMousePosition;

    enum class DragInfo { None, Library, InGame } m_dragInfo;

    int m_dragLibraryIndex{};
    int m_dragFieldIndex{};
    sf::Vector2f m_dragOriginalPosition;
    sf::Vector2f m_dragOffset;

public:
    ImagePresenter(IView& view, GameModel& model, EventManager& document)
        : m_view(view), m_model(model), m_manager(document),
          m_isDragging(false), m_dragInfo(DragInfo::None)
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

    void Update(EventType eventType, const std::optional<sf::Event>& event) override
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

        // Клик по библиотеке
        int libIndex = m_view.GetLibraryIndexAt(mousePressed->position);
        if (libIndex != -1)
        {
            m_dragInfo = DragInfo::Library;
            m_dragLibraryIndex = libIndex;

            m_isDragging = true;
            m_lastMousePosition = mousePressed->position;

            const auto& lib = m_model.GetLibrary();
            const LibraryElement* info = &lib[libIndex];

            sf::Vector2f elemPos = m_view.GetLibraryElementPosition(libIndex);
            auto mousePos = sf::Vector2f(mousePressed->position);
            m_dragOffset = mousePos - elemPos;

            m_view.ShowDraggedElement(info, sf::Vector2f(mousePressed->position), m_dragOffset);
        }

        // Клики в другие места
        int fieldIndex = m_view.GetFieldIndexAt(mousePressed->position);
        if (fieldIndex != -1)
        {
            m_dragInfo = DragInfo::InGame;
            m_dragFieldIndex = fieldIndex;
            m_isDragging = true;
            m_lastMousePosition = mousePressed->position;

            const auto& placed = m_model.GetFieldElements();
            const auto& elem = placed[fieldIndex];
            m_dragOriginalPosition = elem.position;
            m_dragOffset = sf::Vector2f(mousePressed->position) - elem.position;
        }
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
        if (m_dragInfo == DragInfo::InGame)
        {
            sf::Vector2f newPos = sf::Vector2f(currentPos) - m_dragOffset;
            m_view.UpdateFieldElementPosition(m_dragFieldIndex, newPos);
        }
    }

    void OnMouseReleased(const sf::Event& event)
    {
        auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>();
        if (!mouseReleased || !m_isDragging
            || mouseReleased->button != sf::Mouse::Button::Left)
            return;

        auto releasePos = sf::Vector2f(mouseReleased->position);
        sf::FloatRect fieldBounds = m_view.GetFieldBounds();

        int targetIndex = m_view.GetFieldIndexAt(mouseReleased->position);

        // Отпускание в поле
        if (fieldBounds.contains(releasePos))
        {
            // добавить на поле из Lib
            if (m_dragInfo == DragInfo::Library)
            {
                sf::Vector2f dropPos = releasePos - m_dragOffset;
                m_model.AddFieldElement(m_dragLibraryIndex, dropPos);
                m_view.SetFieldElements(m_model.GetFieldElements(), m_model.GetLibrary());
            }
            // перемещение полевого элемента
            if (m_dragInfo == DragInfo::InGame)
            {
                sf::Vector2f dropPos = releasePos - m_dragOffset;
                m_model.UpdateFieldElementPosition(m_dragFieldIndex, dropPos);
                m_view.SetFieldElements(m_model.GetFieldElements(), m_model.GetLibrary());
            }
            // Соединение
        }
        // Отпускание в Lib
        else
        {
            // полевой элемент -> возврат в поле
            if (m_dragInfo == DragInfo::InGame)
            {
                m_model.UpdateFieldElementPosition(m_dragFieldIndex, m_dragOriginalPosition);
                m_view.SetFieldElements(m_model.GetFieldElements(), m_model.GetLibrary());
            }
            // Lib элемент -> возврат в lib
            if (m_dragInfo == DragInfo::Library)
            {
            }
        }

        m_view.HideDraggedElement();
        m_isDragging = false;
        m_dragInfo = DragInfo::None;
    }
};
