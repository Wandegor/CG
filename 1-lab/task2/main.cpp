#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

#include "IShape.h"
#include "PrimitiveShapes/CircleAdapter.h"
#include "PrimitiveShapes/RectangleAdapter.h"
#include "PrimitiveShapes/ConvexAdapter.h"

// Класс для сцены космической ракеты
class RocketScene
{
private:
    std::vector<std::unique_ptr<IShape> > shapes;

public:
    RocketScene()
    {
        CreateScene();
    }

    void CreateScene()
    {
        CreateStars();

        // Луна
        shapes.push_back(std::make_unique<CircleAdapter>(40.0f,
                                                         Color{169, 169, 169},
                                                         Position{100.0f, 500.0f}));

        // Основной корпус
        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 200.0f},
            Color{105, 105, 105},
            Position{375.0f, 200.0f}
        ));

        // Нос
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{400.0f, 150.0f}, // p1: Верхний центр
            Position{375.0f, 200.0f}, // p2: Левый нижний
            Position{425.0f, 200.0f}, // p3: Правый нижний
            Color{220, 220, 220}, // fillColor
            Color{0, 0, 0} // outlineColor
        ));

        // Полосы РОССИЯ
        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},

            Color{255, 255, 255},
            Position{375.0f, 270.0f}
        ));

        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},

            Color{0, 0, 255},
            Position{375.0f, 285.0f}
        ));

        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{50.0f, 15.0f},
            Color{255, 0, 0},
            Position{375.0f, 300.0f}
        ));

        // Иллюминатор
        shapes.push_back(std::make_unique<CircleAdapter>(15.0f,
                                                         Color{30, 144, 255},
                                                         Position{390.0f, 230.0f}));
        shapes.push_back(std::make_unique<CircleAdapter>(8.0f,
                                                         Color{240, 248, 255},
                                                         Position{393.0f, 235.0f}));

        // Хвостовой отсек ракеты (основание)
        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{60.0f, 30.0f},
            Color{192, 192, 192},
            Position{370.0f, 390.0f}
        ));

        // Двигатель
        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{70.0f, 40.0f},
            Color{105, 105, 105},
            Position{365.0f, 420.0f}
        ));

        // Левое крыло
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{360.0f, 380.0f},
            Position{372.0f, 380.0f},
            Position{340.0f, 430.0f},
            Color{169, 169, 169},
            Color{0, 0, 0}
        ));

        // Правое крыло
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{440.0f, 380.0f},
            Position{428.0f, 380.0f},
            Position{460.0f, 430.0f},
            Color{169, 169, 169},
            Color{0, 0, 0}
        ));

        // Антенна
        shapes.push_back(std::make_unique<RectangleAdapter>(
            Position{2.0f, 30.0f},
            Color{192, 192, 192},
            Position{399.0f, 123.0f}
        ));
        shapes.push_back(std::make_unique<CircleAdapter>(
            4.0f,
            Color{255, 215, 0},
            Position{396.0f, 123.0f}
        ));

        createFlames();
    }

    void CreateStars()
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> distX(0, 800);
        std::uniform_real_distribution<float> distY(0, 600);

        for (int i = 0; i < 30; ++i)
        {
            float x = distX(rng);
            float y = distY(rng);

            shapes.push_back(std::make_unique<CircleAdapter>(
                2.0f,
                Color{255, 255, 255},
                Position{x, y}
            ));
        }
    }

    void createFlames()
    {
        // Центральное пламя
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{400.0f, 460.0f},
            Position{390.0f, 510.0f},
            Position{410.0f, 510.0f},
            Color{255, 140, 0},
            Color{0, 0, 0}
        ));

        // Левый
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{380.0f, 460.0f},
            Position{375.0f, 500.0f},
            Position{385.0f, 500.0f},
            Color{255, 69, 0},
            Color{0, 0, 0}
        ));

        // Правый
        shapes.push_back(std::make_unique<ConvexAdapter>(
            Position{420.0f, 460.0f},
            Position{415.0f, 500.0f},
            Position{425.0f, 500.0f},
            Color{255, 69, 0},
            Color{0, 0, 0}
        ));
    }

    void Draw(sf::RenderWindow &window)
    {
        window.clear(sf::Color(10, 10, 40));

        for (const auto &shape: shapes)
        {
            shape->Draw(window);
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Starship rocket ");

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
        }

        scene.Draw(window);

        window.display();
    }

    return 0;
}
