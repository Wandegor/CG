#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

#include "IView.h"
#include "../Listeners/EventManager.h"
#include "../Listeners/EventType.h"

class ImageViewer : public IView
{
private:
    sf::RenderWindow& m_window;
    EventManager& m_manager;

    float m_leftPanelWidth;
    sf::Font m_font;

    std::vector<sf::Sprite> m_librarySprites;
    std::vector<sf::Text> m_libraryTexts;

    std::vector<sf::Sprite> m_fieldSprites;
    std::vector<sf::Text> m_fieldTexts;

    std::optional<sf::Sprite> m_draggedSprite;
    std::optional<sf::Text> m_draggedText;
    sf::Vector2f m_lastDragOffset;
    int m_hiddenFieldIndex;

public:
    ImageViewer(sf::RenderWindow& window, EventManager& document)
        : m_window(window), m_manager(document), m_hiddenFieldIndex(-1)
    {
        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        m_leftPanelWidth = static_cast<float>(m_window.getSize().x) * 0.35f;
    }

    void SetLibrary(std::vector<LibraryElement>& library) override
    {
        if (library.empty()) return;
        m_librarySprites.clear();
        m_libraryTexts.clear();

        const float elemSize = static_cast<float>(library[0].texture.getSize().x);
        constexpr int columns = 4;

        const float xSpacing = m_leftPanelWidth / columns - elemSize;

        const float cellWidth = elemSize + xSpacing;
        const float cellHeight = elemSize + 40;

        for (size_t i = 0; i < library.size(); ++i)
        {
            constexpr float textOffsetY = 3.f;
            const auto& elem = library[i];
            int col = static_cast<int>(i % columns);
            int row = static_cast<int>(i / columns);

            float x = xSpacing / 2 + static_cast<float>(col) * cellWidth;
            float y = 20 + static_cast<float>(row) * cellHeight;

            sf::Sprite sprite(elem.texture);
            sprite.setPosition({x, y});
            m_librarySprites.push_back(sprite);

            sf::Text text(m_font, elem.name);
            text.setCharacterSize(16);
            text.setFillColor(sf::Color::Black);

            sf::FloatRect textBounds = text.getLocalBounds();
            float textX = x + (elemSize - textBounds.size.x) / 2.f;
            float textY = y + elemSize + textOffsetY;
            text.setPosition({textX, textY});

            m_libraryTexts.push_back(text);
        }
    }

    void SetFieldElements(const std::vector<FieldElement>& elements,
                          const std::vector<LibraryElement>& library) override
    {
        m_fieldSprites.clear();
        m_fieldTexts.clear();
        m_hiddenFieldIndex = -1;

        for (const auto& elem: elements)
        {
            const auto& libElem = library[elem.libraryIndex];
            sf::Sprite sprite(libElem.texture);
            sprite.setPosition(elem.position);
            m_fieldSprites.push_back(sprite);

            sf::Text text(m_font, libElem.name);
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::Black);
            sf::FloatRect textBounds = text.getLocalBounds();
            // textX = spriteLeft + (spriteSize.x - textWidth) / 2
            float textX = elem.position.x + (static_cast<float>(libElem.texture.getSize().x) - textBounds.size.x) / 2.f;
            // textY = spriteTop + spriteSize.y + 5
            float textY = elem.position.y + static_cast<float>(libElem.texture.getSize().y) + 5.f;
            text.setPosition({textX, textY});
            m_fieldTexts.push_back(text);
        }
    }

    void UpdateFieldElementPosition(int index, sf::Vector2f newPos) override
    {
        if (index >= 0 && index < m_fieldSprites.size())
        {
            m_fieldSprites[index].setPosition(newPos);

            if (index < m_fieldTexts.size())
            {
                const auto& elem = m_fieldSprites[index];
                sf::FloatRect textBounds = m_fieldTexts[index].getLocalBounds();
                float textX = newPos.x + (static_cast<float>(elem.getTexture().getSize().x) - textBounds.size.x) / 2.f;
                float textY = newPos.y + static_cast<float>(elem.getTexture().getSize().y) + 5.f;
                m_fieldTexts[index].setPosition({textX, textY});
            }
        }
    }

    void ShowDraggedElement(const LibraryElement* element, sf::Vector2f screenPos, sf::Vector2f offset) override
    {
        if (!element) return;
        m_draggedSprite.emplace(element->texture);
        m_draggedSprite->setColor(sf::Color(255, 255, 255));
        m_draggedSprite->setOrigin(offset);
        m_draggedSprite->setPosition(screenPos);

        m_draggedText.emplace(m_font, element->name);
        m_draggedText->setCharacterSize(14);
        m_draggedText->setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = m_draggedText->getLocalBounds();
        sf::Vector2f spriteSize = static_cast<sf::Vector2f>(element->texture.getSize());
        float textX = screenPos.x - offset.x + (spriteSize.x - textBounds.size.x) / 2.f;
        float textY = screenPos.y - offset.y + spriteSize.y + 5.f;
        m_draggedText->setPosition({textX, textY});

        m_lastDragOffset = offset;
    }

    void UpdateDraggedElement(sf::Vector2f screenPos) override
    {
        if (m_draggedSprite.has_value() && m_draggedText.has_value())
        {
            m_draggedSprite->setPosition(screenPos);
            sf::FloatRect textBounds = m_draggedText->getLocalBounds();
            sf::Vector2f spriteSize = static_cast<sf::Vector2f>(m_draggedSprite->getTexture().getSize());
            float textX = screenPos.x - m_lastDragOffset.x + (spriteSize.x - textBounds.size.x) / 2.f;
            float textY = screenPos.y - m_lastDragOffset.y + spriteSize.y + 5.f;
            m_draggedText->setPosition({textX, textY});
        }
    }

    void HideDraggedElement() override
    {
        m_draggedSprite.reset();
        m_draggedText.reset();
    }

    void HideFieldElement(int index) override
    {
        m_hiddenFieldIndex = index;
    }

    sf::Vector2f GetLibraryElementPosition(int index) const override
    {
        if (index >= 0 && index < m_librarySprites.size())
            return m_librarySprites[index].getPosition();
        return {0, 0};
    }

    int GetLibraryIndexAt(sf::Vector2i mousePos) const override
    {
        for (size_t i = 0; i < m_librarySprites.size(); ++i)
        {
            if (m_librarySprites[i].getGlobalBounds().contains(sf::Vector2f(mousePos)))
                return static_cast<int>(i);
        }
        return -1;
    }

    int GetFieldIndexAtIgnoring(sf::Vector2i mousePos, int ignoreIndex) const override
    {
        for (size_t i = 0; i < m_fieldSprites.size(); ++i)
        {
            if (static_cast<int>(i) == ignoreIndex) continue;
            if (m_fieldSprites[i].getGlobalBounds().contains(sf::Vector2f(mousePos)))
                return static_cast<int>(i);
        }
        return -1;
    }

    int GetFieldIndexAt(sf::Vector2i mousePos) const override
    {
        for (size_t i = 0; i < m_fieldSprites.size(); ++i)
        {
            if (m_fieldSprites[i].getGlobalBounds().contains(sf::Vector2f(mousePos)))
                return static_cast<int>(i);
        }
        return -1;
    }

    sf::FloatRect GetFieldBounds() const override
    {
        float left = m_leftPanelWidth;
        float top = 0.f;
        float width = static_cast<float>(m_window.getSize().x) - m_leftPanelWidth;
        float height = static_cast<float>(m_window.getSize().y);
        return {{left, top}, {width, height}};
    }

    void ProcessEvents()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
            }
            else if (auto key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    m_window.close();
                }
            }
            else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    m_manager.NotifyListeners(EventType::MousePressed, event);
                }
            }
            else if (event->getIf<sf::Event::MouseMoved>())
            {
                m_manager.NotifyListeners(EventType::MouseMoved, event);
            }
            else if (event->getIf<sf::Event::MouseButtonReleased>())
            {
                m_manager.NotifyListeners(EventType::MouseReleased, event);
            }
        }
    }

    void Draw()
    {
        m_window.clear(sf::Color(200, 200, 200));

        sf::RectangleShape leftPanel({m_leftPanelWidth, static_cast<float>(m_window.getSize().y)});
        leftPanel.setFillColor(sf::Color(180, 180, 180));
        m_window.draw(leftPanel);

        for (const auto& sprite: m_librarySprites)
            m_window.draw(sprite);
        for (const auto& text: m_libraryTexts)
            m_window.draw(text);

        for (size_t i = 0; i < m_fieldSprites.size(); ++i)
        {
            if (m_hiddenFieldIndex != -1 && m_hiddenFieldIndex == static_cast<int>(i))
                continue;
            m_window.draw(m_fieldSprites[i]);
            m_window.draw(m_fieldTexts[i]);
        }

        if (m_draggedSprite.has_value())
            m_window.draw(*m_draggedSprite);
        if (m_draggedText.has_value())
            m_window.draw(*m_draggedText);
    }

    void Run()
    {
        while (m_window.isOpen())
        {
            ProcessEvents();
            Draw();
            m_window.display();
        }
    }
};
