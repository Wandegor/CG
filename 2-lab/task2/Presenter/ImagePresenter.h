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

    bool m_isDrawing;
    sf::Vector2i m_lastDrawPosition;

public:

    ImagePresenter(IView &view, ImageModel &model, EventManager &document)
            : m_view(view), m_model(model), m_manager(document), m_isDrawing(false)
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

    void Update(EventType eventType, const std::optional<sf::Event> &event) override
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
                OnMousePressed(event.value());
            break;
            case EventType::MouseMoved:
                OnMouseMoved(event.value());
            break;
            case EventType::MouseReleased:
                OnMouseReleased(event.value());
            break;
        }
    }

private:

    void OnNewFile()
    {
        m_model.CreateNew(800, 600, sf::Color::White);
        m_view.SetImage(m_model.GetTexture(), m_model.GetPicturePosition());
    }

    void OnOpenFile()
    {
        auto selection = pfd::open_file("Choose an image", ".",
                                        {"Image Files", "*.jpg *.jpeg"});
        if (!selection.result().empty())
        {
            std::string filename = selection.result()[0];
            if (m_model.LoadFromFile(filename))
            {
                m_view.SetImage(m_model.GetTexture(), m_model.GetPicturePosition());
            }
        }
    }

    void OnSaveFile()
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


        sf::Vector2f picturePos = m_view.GetSpritePosition();
        sf::Vector2i onImagePos(
            mousePressed->position.x - static_cast<int>(picturePos.x),
            mousePressed->position.y - static_cast<int>(picturePos.y)
        );

        // Проверка на попадание по картинке
        if (onImagePos.x >= 0 && onImagePos.y >= 0
            && onImagePos.x < m_model.GetTexture().getSize().x
            && onImagePos.y < m_model.GetTexture().getSize().y)
        {
            m_isDrawing = true;
            m_lastDrawPosition = onImagePos;
            m_view.StartTemporaryStroke(onImagePos);
        }
    }

    void OnMouseMoved(const sf::Event& event)
    {
        auto mouseMoved = event.getIf<sf::Event::MouseMoved>();
        if (!mouseMoved || !m_isDrawing || !m_model.HasImage()) return;

        sf::Vector2f picturePos = m_view.GetSpritePosition();
        sf::Vector2i onImagePos(
            mouseMoved->position.x - static_cast<int>(picturePos.x),
            mouseMoved->position.y - static_cast<int>(picturePos.y)
        );

        if (onImagePos.x >= 0 && onImagePos.y >= 0
            && onImagePos.x < m_model.GetTexture().getSize().x
            && onImagePos.y < m_model.GetTexture().getSize().y)
        {
            m_view.AddTemporaryPoint(m_lastDrawPosition, onImagePos);
            m_lastDrawPosition = onImagePos;
        }
    }

    void OnMouseReleased(const sf::Event& event)
    {
        auto mouseReleased = event.getIf<sf::Event::MouseButtonReleased>();
        if (!mouseReleased) return;

        if (mouseReleased->button == sf::Mouse::Button::Left && m_isDrawing)
        {
            const sf::Texture& strokeTexture = m_view.FinishTemporaryStroke();
            m_model.UpdateTexture(strokeTexture);
            m_isDrawing = false;
        }
    }
};
