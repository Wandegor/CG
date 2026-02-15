#pragma once
#include "../Listeners/Document.h"
#include "../Model/ImageModel.h"
#include "../../portable-file-dialogs.h"
#include "../View/ImageViewer.h"

class ImagePresenter : public IDocumentListener
{
private:
    IView &m_view;
    ImageModel &m_model;
    Document &m_document;

    bool m_isDragging;
    sf::Vector2f m_dragStartPosition;
    sf::Vector2f m_modelStartPos;

public:

    ImagePresenter(IView &view, ImageModel &model, Document &document)
        : m_view(view), m_model(model), m_document(document), m_isDragging(false)
    {
        m_document.Subscribe("openFile", *this);
    }

    virtual ~ImagePresenter()
    {
        m_document.Unsubscribe("openFile", *this);
    }

    void Update(const std::string &eventType, void *data) override
    {
        if (eventType == "openFile")
        {
            OnOpenFile();
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
};
