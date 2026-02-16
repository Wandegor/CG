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

    void Update(const std::string &eventType, void *data) override
    {
        if (eventType == "openFile")
        {
            OnOpenFile();
        } else if (eventType == "mousePressed")
        {
            OnMousePressed(static_cast<MouseData *>(data));
        } else if (eventType == "mouseMoved")
        {
            OnMouseMoved(static_cast<sf::Vector2i *>(data));
        } else if (eventType == "mouseReleased")
        {
            OnMouseReleased(static_cast<sf::Mouse::Button *>(data));
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

    void OnMousePressed(MouseData *mouseData)
    {
        if (mouseData->button != sf::Mouse::Button::Left) return;

        if (m_model.GetRect().contains(mouseData->pos))
        {
            m_isDragging = true;
            m_lastMousePosition = {
                    static_cast<float>(mouseData->pos.x),
                    static_cast<float>(mouseData->pos.y)};

        }
    }

    void OnMouseMoved(sf::Vector2i *pos)
    {
        if (!m_isDragging) return;

        sf::Vector2f currentPos(static_cast<float>(pos->x),
                                static_cast<float>(pos->y));
        sf::Vector2f delta = currentPos - m_lastMousePosition;

        m_model.Move(delta);
        m_view.MoveImage(delta);

        m_lastMousePosition = currentPos;
                
    }

    void OnMouseReleased(const sf::Mouse::Button *button)
    {
        if (*button == sf::Mouse::Button::Left)
        {
            m_isDragging = false;
        }
    }
};
