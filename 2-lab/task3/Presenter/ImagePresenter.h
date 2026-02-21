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

            // Призрак пол элемента
            m_view.HideFieldElement(fieldIndex);

            const auto& lib = m_model.GetLibrary();
            const LibraryElement* info = &lib[elem.libraryIndex];
            m_view.ShowDraggedElement(info, sf::Vector2f(mousePressed->position), m_dragOffset);
        }
    }

    void OnMouseMoved(const sf::Event& event)
    {
        auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
        if (!mouseMoved) return;
        if (!m_isDragging) return;

        sf::Vector2i currentPos(mouseMoved->position.x, mouseMoved->position.y);
        if (m_dragInfo == DragInfo::Library || m_dragInfo == DragInfo::InGame)
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

        auto releasePos = sf::Vector2f(mouseReleased->position);
        sf::FloatRect fieldBounds = m_view.GetFieldBounds();
        float iconSize = m_view.GetIconSize();

        sf::Vector2f dropPos = releasePos - m_dragOffset;
        sf::FloatRect dropRect(dropPos, {iconSize, iconSize});

        // Drop в Lib
        if (!fieldBounds.contains(releasePos))
        {
            // полевой элемент -> возврат в поле
            // Lib элемент -> возврат в lib
            m_view.SetFieldElements(m_model.GetFieldElements(), m_model.GetLibrary());
            m_view.HideDraggedElement();
            m_isDragging = false;
            m_dragInfo = DragInfo::None;
            return;
        }

        // Drop в поле (ищем targetIndex)
        const auto& fieldElements = m_model.GetFieldElements();
        int targetIndex = -1;
        for (int i = 0; i < static_cast<int>(fieldElements.size()); ++i)
        {
            if (m_dragInfo == DragInfo::InGame && i == m_dragFieldIndex)
            {
                continue; // дроп на себя
            }
            sf::FloatRect elemRect(fieldElements[i].position, {iconSize, iconSize});
            if (dropRect.findIntersection(elemRect).has_value())
            {
                targetIndex = i;
                break;
            }
        }

        // Нет пересечения
        if (targetIndex == -1)
        {
            float leftBound = fieldBounds.position.x;
            sf::Vector2f finalPos = dropPos;
            // чтобы спрайт не вылазил за игровое поле или lib
            if (finalPos.x < leftBound)
                finalPos.x = leftBound + 10;

            if (m_dragInfo == DragInfo::Library)
                // Добавить на поле из Lib
                m_model.AddFieldElement(m_dragLibraryIndex, finalPos);
            else if (m_dragInfo == DragInfo::InGame)
                // Перемещение полевого элемента
                m_model.UpdateFieldElementPosition(m_dragFieldIndex, finalPos);
        }
        else // Соединение с targetIndex
        {
            // добавить на поле из Lib (пока что)
            if (m_dragInfo == DragInfo::Library)
            {
                m_model.AddFieldElement(m_dragLibraryIndex, dropPos);
            }
            // Соединение полевых
            if (m_dragInfo == DragInfo::InGame)
            {
                int sourceIdx = m_model.GetFieldElements()[m_dragFieldIndex].libraryIndex;
                int targetIdx = m_model.GetFieldElements()[targetIndex].libraryIndex;

                std::vector<int> results;
                if (m_model.GetCombinationResult(sourceIdx, targetIdx, results))
                {
                    int first = std::max(m_dragFieldIndex, targetIndex);
                    int second = std::min(m_dragFieldIndex, targetIndex);
                    m_model.RemoveFieldElement(first);
                    m_model.RemoveFieldElement(second);
                    for (size_t i = 0; i < results.size(); i++)
                    {
                        m_model.AddFieldElement(results[i], {
                            dropPos.x + static_cast<float>(i)*(iconSize+5),
                            dropPos.y});
                    }
                }
                else // нет такой комбинации - возврат обратно
                {
                    m_model.UpdateFieldElementPosition(m_dragFieldIndex, m_dragOriginalPosition);
                }
            }
        }

        m_view.SetFieldElements(m_model.GetFieldElements(), m_model.GetLibrary());
        m_view.HideDraggedElement();
        m_isDragging = false;
        m_dragInfo = DragInfo::None;
    }
};
