#pragma once

#include "../Listeners/IEventListener.h"
#include "../Model/ImageModel.h"

#include "../View/ImageViewer.h"
#include <portable-file-dialogs.h>

#include "../../task1/Listeners/EventType.h"

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
        m_manager.Subscribe(EventType::NewFile, *this);
        m_manager.Subscribe(EventType::OpenFile, *this);
        m_manager.Subscribe(EventType::SaveFile, *this);
        m_manager.Subscribe(EventType::MousePressed, *this);
        m_manager.Subscribe(EventType::MouseMoved, *this);
        m_manager.Subscribe(EventType::MouseReleased, *this);
    }

    virtual ~ImagePresenter()
    {
        m_manager.Unsubscribe(EventType::NewFile, *this);
        m_manager.Unsubscribe(EventType::OpenFile, *this);
        m_manager.Unsubscribe(EventType::SaveFile, *this);
        m_manager.Unsubscribe(EventType::MousePressed, *this);
        m_manager.Unsubscribe(EventType::MouseMoved, *this);
        m_manager.Unsubscribe(EventType::MouseReleased, *this);
    }

    void Update(EventType eventType, const sf::Event& event) override
    {
        switch (eventType)
        {
            case EventType::NewFile:
                OnNewFile();
            break;
            case EventType::OpenFile:
                OnOpenFile();
            break;
            case EventType::SaveFile:
                OnSaveFile();
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
        }
    }

private:

    void OnNewFile()
    {
        m_model.CreateNew(800, 600, sf::Color::White);
        m_view.SetImage(m_model.GetTexture(), m_model.GetRect());
    }

    void OnOpenFile() const
    {
        auto selection = pfd::open_file("Choose an image", ".",
                                        {"Image Files", "*.jpg *.jpeg"});
        if (!selection.result().empty())
        {
            std::string filename = selection.result()[0];
            if (m_model.LoadFromFile(filename))
            {
                m_view.SetImage(m_model.GetTexture(), m_model.GetRect());
            }
        }
    }

    void OnSaveFile() const
    {
        if (m_model.GetTexture().getSize().x == 0)
        {
            std::cerr << "No image to save" << std::endl;
            return;
        }

        auto selection = pfd::save_file("Save image", ".",
                                    {"Image Files", "*.jpg *.jpeg"});

        if (!selection.result().empty())
        {
            std::string file = selection.result();
            if (!m_model.SaveToFile(file))
            {
                std::cerr << "Failed to save image" << std::endl;
            }
        }
    }

    void OnMousePressed(const sf::Event& event)
    {
        auto mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        if (!mousePressed) return;
        if (mousePressed->button != sf::Mouse::Button::Left) return;

        if (!m_model.HasImage()) return;


        auto& rect = m_model.GetRect();
        sf::Vector2f imagePos = {
            static_cast<float>(mousePressed->position.x - rect.position.x),
            static_cast<float>(mousePressed->position.y - rect.position.y)};
        // // Проверяем, что клик внутри изображения
        // if (imagePos.x >= 0 && imagePos.x < (int)m_model.GetImageSize().x &&
        //     imagePos.y >= 0 && imagePos.y < (int)m_model.GetImageSize().y)
        if (rect.contains(mousePressed->position))
        {
            m_isDragging = true;
            m_lastMousePosition = imagePos;
            // Ставим начальную точку
            m_model.DrawPoint(sf::Vector2i(imagePos), sf::Color::Black);
            m_model.UpdateTexture();
        }
    }

    void OnMouseMoved(const sf::Event& event)
    {
        auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
        if (!mouseMoved) return;
        if (!m_isDragging) return;
        if (!m_model.HasImage()) return;

        auto& rect = m_model.GetRect();
        sf::Vector2i currentPos = {
            mouseMoved->position.x - rect.position.x,
            mouseMoved->position.y - rect.position.y};

        if (currentPos.x < 0 || currentPos.x >= m_model.GetRect().size.x ||
            currentPos.y < 0 || currentPos.y >= m_model.GetRect().size.y)
        {
            m_isDragging = false;
            return;
        }

        m_model.DrawLine(sf::Vector2i(m_lastMousePosition), currentPos, sf::Color::Black);
        m_lastMousePosition = sf::Vector2f(currentPos);
        m_model.UpdateTexture();
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
