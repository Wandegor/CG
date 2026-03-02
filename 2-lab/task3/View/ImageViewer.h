#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "../ViewComponents/Button.h"
#include "../ViewComponents/Menu.h"
#include "IView.h"
#include "../Listeners/EventManager.h"
#include "../Listeners/EventType.h"

class ImageViewer : public IView
{
private:
    sf::RenderWindow& m_window;
    EventManager& m_manager;

    sf::Vector2f m_baseSize;
    sf::View m_gameView;
    sf::View m_uiView;
    std::vector<LibraryElement> m_lastLibrary;
    std::vector<int> m_lastUnlockedIndices;
    std::vector<FieldElement> m_lastFieldElements;

    std::unique_ptr<Menu> m_musicMenu;
    sf::Music m_backgroundMusic;
    std::string m_currentMusicPath;

    float m_leftPanelWidth;

    sf::Font m_font;

    std::unique_ptr<Button> m_sortButton;

    std::vector<sf::Sprite> m_librarySprites;
    std::vector<sf::Text> m_libraryTexts;

    std::vector<sf::Sprite> m_fieldSprites;
    std::vector<sf::Text> m_fieldTexts;

    sf::Texture m_crossTexture;
    std::optional<sf::Sprite> m_removeSprite;
    float m_iconSize;

    std::optional<sf::Sprite> m_draggedSprite;
    std::optional<sf::Text> m_draggedText;
    sf::Vector2f m_lastDragOffset;
    int m_hiddenFieldIndex;

    sf::Clock m_gameOverClock;
    std::optional<sf::Text> m_gameOverText;

public:
    ImageViewer(sf::RenderWindow& window, EventManager& document)
        : m_window(window), m_manager(document),
          m_hiddenFieldIndex(-1), m_iconSize(80.f), m_baseSize(1200.f, 800.f)
    {
        if (m_backgroundMusic.openFromFile("Resources/Sounds/LumierExpedition.mp3"))
        {
            m_backgroundMusic.setLooping(true);
            m_backgroundMusic.setVolume(6.f);
            m_currentMusicPath = "Resources/Sounds/LumierExpedition.mp3";
        }
        else
        {
            std::cerr << "Failed to load background music" << std::endl;
        }

        if (!m_font.openFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        m_leftPanelWidth = static_cast<float>(m_window.getSize().x) * 0.4f;
        m_sortButton = std::make_unique<Button>(
            m_font,
            "Sort",
            sf::Vector2f(m_leftPanelWidth / 2 - 50, static_cast<float>(m_window.getSize().y) - 75),
            sf::Vector2f(100, 50),
            [this]() { m_manager.NotifyListeners(EventType::SortLibrary); }
        );

        if (!m_crossTexture.loadFromFile("Resources/removeElementCross.png"))
        {
            std::cerr << "Error loading removeElementCross.png" << std::endl;
        }
        m_removeSprite.emplace(m_crossTexture);
        m_removeSprite->setPosition(sf::Vector2f(
            m_leftPanelWidth + (static_cast<float>(m_window.getSize().x) - m_leftPanelWidth) / 2,
            static_cast<float>(m_window.getSize().y) - 125));
        std::cout << m_removeSprite->getPosition().x << std::endl;

        m_gameView.setSize(m_baseSize);
        m_gameView.setCenter(m_baseSize / 2.f);

        m_uiView.setSize(sf::Vector2f(window.getSize()));
        m_uiView.setCenter(sf::Vector2f(window.getSize()) / 2.f);

        m_musicMenu = std::make_unique<Menu>(
                m_font,
                "Music",
                sf::Vector2f(static_cast<float>(m_window.getSize().x) - 20 - 100, 20),
                sf::Vector2f(100, 40));

        m_musicMenu->AddItem("Lumier", [this]() {
            ToggleMusic("Resources/Sounds/LumierExpedition.mp3");
        });

        m_musicMenu->AddItem("ZIzazay", [this]() {
            ToggleMusic("Resources/Sounds/zyi-zai-zui-zai-zy.mp3");
        });
    }

    void SetLibrary(const std::vector<LibraryElement>& library, const std::vector<int>& unlockedIndices) override
    {
        m_lastLibrary = library;
        m_lastUnlockedIndices = unlockedIndices;

        if (library.empty()) return;
        m_librarySprites.clear();
        m_libraryTexts.clear();

        const int columns = static_cast<int>(m_leftPanelWidth / (m_iconSize * 1.2f));

        const float xSpacing = m_leftPanelWidth / columns - m_iconSize;

        const float cellWidth = m_iconSize + xSpacing;
        const float cellHeight = m_iconSize + 40;

        for (size_t i = 0; i < unlockedIndices.size(); ++i)
        {
            int libIndex = unlockedIndices[i];
            constexpr float textOffsetY = 3.f;
            const auto& elem = library[libIndex];
            int col = static_cast<int>(i % columns);
            int row = static_cast<int>(i / columns);

            float x = xSpacing / 2 + static_cast<float>(col) * cellWidth;
            float y = 20 + static_cast<float>(row) * cellHeight;

            sf::Sprite sprite(elem.texture);
            sf::Vector2u size = elem.texture.getSize();
            float scaleX = m_iconSize / static_cast<float>(size.x);
            float scaleY = m_iconSize / static_cast<float>(size.y);
            sprite.setScale({scaleX, scaleY});
            sprite.setPosition({x, y});
            m_librarySprites.push_back(sprite);

            sf::Text text(m_font, elem.name);
            text.setFillColor(sf::Color::Black);
            text.setCharacterSize(18);

            sf::FloatRect textBounds = text.getLocalBounds();

            float textX = x + (m_iconSize - textBounds.size.x) / 2.f;
            float textY = y + m_iconSize + textOffsetY;
            text.setPosition({textX, textY});

            m_libraryTexts.push_back(text);
        }
    }

    void SetFieldElements(const std::vector<FieldElement>& elements,
                          const std::vector<LibraryElement>& library) override
    {
        m_lastFieldElements = elements;
        m_fieldSprites.clear();
        m_fieldTexts.clear();
        m_hiddenFieldIndex = -1;

        for (const auto& elem: elements)
        {
            const auto& libElem = library[elem.libraryIndex];
            sf::Sprite sprite(libElem.texture);
            sf::Vector2u texSize = libElem.texture.getSize();
            float scaleX = m_iconSize / static_cast<float>(texSize.x);
            float scaleY = m_iconSize / static_cast<float>(texSize.y);
            sprite.setScale({scaleX, scaleY});
            sprite.setPosition({m_leftPanelWidth + elem.position.x, elem.position.y});
            m_fieldSprites.push_back(sprite);

            sf::Vector2f spriteSize = sprite.getGlobalBounds().size;
            sf::Text text(m_font, libElem.name);
            text.setFillColor(sf::Color::Black);
            text.setCharacterSize(18);
            sf::FloatRect textBounds = text.getLocalBounds();
            // textX = spriteLeft + (spriteSize.x - textWidth) / 2
            float textX = m_leftPanelWidth + elem.position.x + (spriteSize.x - textBounds.size.x) / 2.f;
            // textY = spriteTop + spriteSize.y + 5
            float textY = elem.position.y + spriteSize.y + 5.f;
            text.setPosition({textX, textY});
            m_fieldTexts.push_back(text);
        }
    }

    void UpdateFieldElementPosition(int index, sf::Vector2f newPos) override
    {
        if (index >= 0 && index < m_fieldSprites.size())
        {
            m_fieldSprites[index].setPosition({m_leftPanelWidth + newPos.x, newPos.y});

            if (index < m_fieldTexts.size())
            {
                const auto& sprite = m_fieldSprites[index];
                sf::Vector2f spriteSize = sprite.getGlobalBounds().size;
                sf::FloatRect textBounds = m_fieldTexts[index].getLocalBounds();
                float textX = newPos.x + (spriteSize.x - textBounds.size.x) / 2.f;
                float textY = newPos.y + spriteSize.y + 5.f;
                m_fieldTexts[index].setPosition({textX, textY});
            }
        }
    }

    void ShowDraggedElement(const LibraryElement* element, sf::Vector2f screenPos, sf::Vector2f offset) override
    {
        if (!element) return;
        m_draggedSprite.emplace(element->texture);
        m_draggedSprite->setColor(sf::Color(255, 255, 255));
        sf::Vector2u texSize = element->texture.getSize();
        float scaleX = m_iconSize / static_cast<float>(texSize.x);
        float scaleY = m_iconSize / static_cast<float>(texSize.y);
        m_draggedSprite->setScale({scaleX, scaleY});
        m_draggedSprite->setPosition(screenPos - offset);

        m_draggedText.emplace(m_font, element->name);
        m_draggedText->setCharacterSize(18);
        m_draggedText->setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = m_draggedText->getLocalBounds();

        sf::Vector2f spriteSize = m_draggedSprite->getGlobalBounds().size;
        float textX = screenPos.x - offset.x + (spriteSize.x - textBounds.size.x) / 2.f;
        float textY = screenPos.y - offset.y + spriteSize.y + 5.f;
        m_draggedText->setPosition({textX, textY});

        m_lastDragOffset = offset;
    }

    void UpdateDraggedElement(sf::Vector2f screenPos) override
    {
        if (m_draggedSprite.has_value() && m_draggedText.has_value())
        {
            m_draggedSprite->setPosition(screenPos - m_lastDragOffset);
            sf::FloatRect textBounds = m_draggedText->getLocalBounds();
            sf::Vector2f spriteSize = m_draggedSprite->getGlobalBounds().size;
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

    std::wstring GetLibraryElementNameAt(sf::Vector2i mousePos) const override
    {
        for (size_t i = 0; i < m_librarySprites.size(); ++i)
        {
            if (m_librarySprites[i].getGlobalBounds().contains(sf::Vector2f(mousePos)))
                return m_libraryTexts[i].getString();
        }
        return L"";
    }

    sf::Vector2f GetLibraryElementPosition(int index) const override
    {
        if (index >= 0 && index < m_librarySprites.size())
            return m_librarySprites[index].getPosition();
        return {0, 0};
    }

    sf::FloatRect GetFieldBounds() const override
    {
        float left = m_leftPanelWidth;
        float top = 0.f;
        float width = static_cast<float>(m_window.getSize().x) - m_leftPanelWidth;
        float height = static_cast<float>(m_window.getSize().y);
        return {{left, top}, {width, height}};
    }

    float GetLeftPanelWidth() const override {
        return m_leftPanelWidth;
    }

    sf::FloatRect GetDelElemBounds() const override
    {
        if (m_removeSprite.has_value())
            return m_removeSprite->getGlobalBounds();
        return {};
    }

    float GetIconSize() const override
    {
        return m_iconSize;
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
                    if (m_sortButton->Contains(mousePressed->position))
                    {
                        m_sortButton->OnClick();
                        continue;
                    }
                    m_manager.NotifyListeners(EventType::MousePressed, event);
                }
            }
            else if (event->getIf<sf::Event::MouseMoved>())
            {
                m_manager.NotifyListeners(EventType::MouseMoved, event);
            }
            else if (auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                m_manager.NotifyListeners(EventType::MouseReleased, event);
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::Vector2f newSize = sf::Vector2f(m_window.getSize());

                m_uiView.setSize(newSize);
                m_uiView.setCenter(newSize / 2.f);

                float leftPanelWidth = newSize.x * 0.4f;

                m_sortButton->SetPosition(sf::Vector2f(leftPanelWidth / 2 - 50, newSize.y - 75));

                if (m_removeSprite.has_value()) {
                    m_removeSprite->setPosition(sf::Vector2f(
                        leftPanelWidth + (newSize.x - leftPanelWidth) / 2,
                        newSize.y - 125
                    ));
                }

                m_leftPanelWidth = leftPanelWidth;

                if (!m_lastLibrary.empty() && !m_lastUnlockedIndices.empty()) {
                    SetLibrary(m_lastLibrary, m_lastUnlockedIndices);
                }
                if (!m_lastFieldElements.empty() && !m_lastLibrary.empty()) {
                    SetFieldElements(m_lastFieldElements, m_lastLibrary);
                }
            }

            m_musicMenu->HandleEvent(*event, m_window);
        }
    }

    void Draw()
    {
        m_window.clear(sf::Color(200, 200, 200));

        m_window.setView(m_uiView);

        sf::RectangleShape leftPanel({m_leftPanelWidth, static_cast<float>(m_window.getSize().y)});
        leftPanel.setFillColor(sf::Color(180, 180, 180));
        m_window.draw(leftPanel);

        for (const auto& sprite: m_librarySprites)
            m_window.draw(sprite);
        for (const auto& text: m_libraryTexts)
            m_window.draw(text);

        // Кнопка сортировки
        m_sortButton->DrawTo(m_window);

        // Крестик
        if (m_removeSprite.has_value())
            m_window.draw(*m_removeSprite);

        // m_window.setView(m_gameView);
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

        m_musicMenu->Draw(m_window);
        if (m_gameOverText.has_value()) {
            m_window.draw(*m_gameOverText);
            if (m_gameOverClock.getElapsedTime().asSeconds() > 4.0f) {
                m_gameOverText.reset();
            }
        }
    }

    void Run()
    {
        m_backgroundMusic.play();
        while (m_window.isOpen())
        {
            ProcessEvents();
            Draw();
            m_window.display();
        }
    }

    void ShowGameOverMessage(const std::wstring& message) override {
        m_gameOverText.emplace(m_font, message);
        m_gameOverText->setCharacterSize(40);
        m_gameOverText->setFillColor(sf::Color::Green);
        m_gameOverText->setPosition({
            static_cast<float>(m_window.getSize().x)/2,
            static_cast<float>(m_window.getSize().y)/2});
        m_gameOverClock.restart();
    }

private:
    void ToggleMusic(const std::string& path)
    {
        if (m_currentMusicPath == path)
        {
            // Тот же трек – переключаем паузу
            if (m_backgroundMusic.getStatus() == sf::Music::Status::Playing)
                m_backgroundMusic.pause();
            else
                m_backgroundMusic.play();
        }
        else
        {
            // Другой трек – загружаем и запускаем
            if (m_backgroundMusic.openFromFile(path))
            {
                m_backgroundMusic.setLooping(true);
                m_backgroundMusic.setVolume(6.f);
                m_backgroundMusic.play();
                m_currentMusicPath = path;
            }
            else
            {
                std::cerr << "Failed to load " << path << std::endl;
            }
        }
    }
};
