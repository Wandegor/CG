#pragma once

#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Circle.h"


class Canvas
{
    sf::RenderWindow &m_window;


    void DrawPixel(int x, int y, Color color)
    {
        if (x >= 0 && x < static_cast<int>(m_window.getSize().x) &&
            y >= 0 && y < static_cast<int>(m_window.getSize().y))
        {
            sf::Vertex point(
             sf::Vector2f(x, y),
             sf::Color(color.r, color.g, color.b));
            m_window.draw(&point, 1, sf::PrimitiveType::Points);
        }
    }

    void Add8Points(int xc, int yc, int x, int y, Color color)
    {
        DrawPixel(xc + x, yc + y, color);
        DrawPixel(xc - x, yc + y, color);
        DrawPixel(xc + x, yc - y, color);
        DrawPixel(xc - x, yc - y, color);
        DrawPixel(xc + y, yc + x, color);
        DrawPixel(xc - y, yc + x, color);
        DrawPixel(xc + y, yc - x, color);
        DrawPixel(xc - y, yc - x, color);
    }

    void DrawCircleBresenham(int xc,int yc, int r,Color color)
    {
        int x = 0;
        int y = r;
        int d = 3 - 2 * r;

        Add8Points(xc, yc, x, y, color);
        while (y >= x)
        {
            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            } else
                d = d + 4 * x + 6;

            x++;

            Add8Points(xc, yc, x, y, color);

            // std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // m_window.display();
        }
    }

    void DrawThickCircleNormal(Circle circle)
    {
        int xc = circle.GetPosition().m_x;
        int yc = circle.GetPosition().m_y;
        int radius = circle.GetRadius();

        int halfThickness = circle.GetOutThickness() / 2;
        int outerRadius = radius + halfThickness;
        int innerRadius = std::max(0, radius - halfThickness);

        // Рисуем две окружности
        DrawCircleBresenham(xc, yc, outerRadius, circle.GetOutlineColor());
        DrawCircleBresenham(xc, yc, innerRadius, circle.GetOutlineColor());
    }
public:
    Canvas(sf::RenderWindow &window)
        : m_window(window) {}

    void Draw(Circle circle)
    {
        DrawThickCircleNormal(circle);
    }
};
