#pragma once
#include "../Listeners/Document.h"
#include "../Model/ImageModel.h"
#include "../../portable-file-dialogs.h"
#include "../View/ImageViewer.h"

class ImageViewer;

struct MouseData
{
    sf::Vector2f pos;
    sf::Mouse::Button button;
};

class ImagePresenter : public IDocumentListener
{
private:
    ImageViewer &m_view;
    ImageModel &m_model;
    Document &m_document;

    bool m_isDragging;
    sf::Vector2f m_dragStartPosition;
    sf::Vector2f m_modelStartPos;

public:
    virtual ~ImagePresenter() {}

    ImagePresenter(ImageViewer &view, ImageModel &model, Document &document)
        : m_view(view), m_model(model), m_document(document), m_isDragging(false)
    {
        m_document.Subscribe("mousePressed", *this);
        m_document.Subscribe("mouseMoved", *this);
        m_document.Subscribe("mouseReleased", *this);
        m_document.Subscribe("openFile", *this);
    }

    void Update(const std::string &eventType, void *data) override
    {
        if (eventType == "mousePressed")
        {
            OnMousePressed(static_cast<MouseData*>(data));
        } else if (eventType == "mouseMoved")
        {
            OnMouseMoved(static_cast<sf::Vector2f*>(data));
        } else if (eventType == "mouseReleased")
        {
            OnMouseReleased(static_cast<sf::Mouse::Button*>(data));
        } else if (eventType == "openFile")
        {
            OnOpenFile();
        }
    }

private:
    void OnMousePressed(MouseData *mouseData)
    {
        // if (!m_model.hasImage()) return;
        if (mouseData->button != sf::Mouse::Button::Left) return;

        sf::FloatRect spriteBounds = m_model.GetSprite().getGlobalBounds();

        if (spriteBounds.contains(mouseData->pos))
        {
            m_isDragging = true;
            m_dragStartPosition = mouseData->pos;
            m_modelStartPos = m_model.GetPosition();
        }
    }

    void OnMouseMoved(sf::Vector2f *pos)
    {
        if (!m_isDragging) return;

        sf::Vector2f delta = {
            pos->x - m_dragStartPosition.x,
            pos->y - m_dragStartPosition.y
        };
        m_model.Move(delta);

        m_view.UpdateImage(m_model.GetSprite());
    }

    void OnMouseReleased(const sf::Mouse::Button *button)
    {
        if (*button == sf::Mouse::Button::Left)
        {
            m_isDragging = false;
        }
    }

    void OnOpenFile()
    {
        auto selection = pfd::open_file("Choose an image", ".",
                                       {"Image Files", "*.jpg *.jpeg *.png *.bmp"});
        if (!selection.result().empty())
        {
            std::string filename = selection.result()[0];
            if (m_model.LoadFromFile(filename))
            {
                m_view.UpdateImage(m_model.GetSprite());
            }
        }
    }
};
