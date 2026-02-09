#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

#include "IShape.h"
#include "PrimitiveShapes/CircleAdapter.h"
#include "PrimitiveShapes/RectangleAdapter.h"
#include "PrimitiveShapes/ConvexAdapter.h"

class RocketScene
{
private:
    std::vector<std::unique_ptr<IShape> > m_shapes;
    sf::Vector2f offset; // Смещение
    bool isDragging;
    sf::Vector2f dragStartPosition;

public:
    RocketScene() : offset(0, 0), isDragging(false), dragStartPosition(0, 0)
    {
        CreateScene();
    }

    void CreateScene()
    {
        CreateStars();

        // Луна
        m_shapes.push_back(std::make_unique<CircleAdapter>(40.0f,
                                                           Color{169, 169, 169},
                                                           Position{100.0f, 500.0f}));

        // Основной корпус
        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 200.0f},
            Color{105, 105, 105},
            Position{375.0f, 200.0f}
        ));

        // Нос
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{400.0f, 150.0f},
            Position{375.0f, 200.0f},
            Position{425.0f, 200.0f},
            Color{220, 220, 220},
            Color{0, 0, 0}
        ));

        // Полосы РОССИЯ
        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},

            Color{255, 255, 255},
            Position{375.0f, 270.0f}
        ));

        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},

            Color{0, 0, 255},
            Position{375.0f, 285.0f}
        ));

        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},
            Color{255, 0, 0},
            Position{375.0f, 300.0f}
        ));

        // Иллюминатор
        m_shapes.push_back(std::make_unique<CircleAdapter>(15.0f,
                                                           Color{30, 144, 255},
                                                           Position{390.0f, 230.0f}));
        m_shapes.push_back(std::make_unique<CircleAdapter>(8.0f,
                                                           Color{240, 248, 255},
                                                           Position{393.0f, 235.0f}));

        // Хвост
        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{60.0f, 30.0f},
            Color{192, 192, 192},
            Position{370.0f, 390.0f}
        ));

        // Двигатель
        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{70.0f, 40.0f},
            Color{105, 105, 105},
            Position{365.0f, 420.0f}
        ));

        // Левое крыло
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{360.0f, 380.0f},
            Position{372.0f, 380.0f},
            Position{340.0f, 430.0f},
            Color{169, 169, 169},
            Color{0, 0, 0}
        ));

        // Правое крыло
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{440.0f, 380.0f},
            Position{428.0f, 380.0f},
            Position{460.0f, 430.0f},
            Color{169, 169, 169},
            Color{0, 0, 0}
        ));

        // Антенна
        m_shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{2.0f, 30.0f},
            Color{192, 192, 192},
            Position{399.0f, 123.0f}
        ));
        m_shapes.push_back(std::make_unique<CircleAdapter>(
            4.0f,
            Color{255, 215, 0},
            Position{396.0f, 123.0f}
        ));

        CreateFlames();
    }

    void CreateStars()
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> distX(0, 800);
        std::uniform_real_distribution<float> distY(0, 800);

        for (int i = 0; i < 30; ++i)
        {
            float x = distX(rng);
            float y = distY(rng);

            m_shapes.push_back(std::make_unique<CircleAdapter>(
                2.0f,
                Color{255, 255, 255},
                Position{x, y}
            ));
        }
    }

    void CreateFlames()
    {
        // Центральное пламя
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{400.0f, 460.0f},
            Position{390.0f, 510.0f},
            Position{410.0f, 510.0f},
            Color{255, 140, 0},
            Color{0, 0, 0}
        ));

        // Левый
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{380.0f, 460.0f},
            Position{375.0f, 500.0f},
            Position{385.0f, 500.0f},
            Color{255, 69, 0},
            Color{0, 0, 0}
        ));

        // Правый
        m_shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{420.0f, 460.0f},
            Position{415.0f, 500.0f},
            Position{425.0f, 500.0f},
            Color{255, 69, 0},
            Color{0, 0, 0}
        ));
    }

    void HandleEvent(const sf::Event &event)
    {
        if (const auto *mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left)
            {
                isDragging = true;
                dragStartPosition.x = static_cast<float>(mouseButtonPressed->position.x);
                dragStartPosition.y = static_cast<float>(mouseButtonPressed->position.y);
            }
        } else if (const auto *mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseButtonReleased->button == sf::Mouse::Button::Left)
            {
                isDragging = false;
            }
        } else if (const auto *mouseMove = event.getIf<sf::Event::MouseMoved>())
        {
            if (isDragging)
            {
                sf::Vector2f currentPosition(static_cast<float>(mouseMove->position.x),
                                             static_cast<float>(mouseMove->position.y));
                sf::Vector2f delta = currentPosition - dragStartPosition;

                // Новое смещение
                offset -= delta;
                dragStartPosition = currentPosition;
            }
        }
    }

    void Draw(sf::RenderWindow &window)
    {
        window.clear(sf::Color(10, 10, 40));

        sf::View view = window.getView();

        view.move(offset);
        window.setView(view);

        for (const auto &shape: m_shapes)
        {
            shape->Draw(window);
        }

        // Восстанавливает вид
        view.move(-offset);
        window.setView(view);
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Starship rocket ");

    RocketScene scene;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Escape)
                    window.close();
            }

            scene.HandleEvent(*event);
        }

        scene.Draw(window);

        window.display();
    }

    return 0;
}
