#pragma once

#include <complex>
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

    void DrawCircleBresenham(int xc, int yc, int r, Color color)
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

    void FillBetweenCircles(int xc, int yc, int inR, int outR, Color color)
    {
        // (x - xc)² + (y - yc)² = R²

        // С верху окружности до низа (внешней)
        for (int y = yc - outR; y <= yc + outR; y++)
        {
            // Вертикальное смещение от центра
            int dy = y - yc;

            // (y - yc)² ≤ R²      Если истина, значит строка y пересекает окружность
            if (dy * dy <= outR * outR)
            {
                // x = xc ± √ (R² - (y - yc)²)
                // Горизонтальное расстояние от центра до внешней окружности

                int dxOuter = static_cast<int>(std::sqrt(outR * outR - dy * dy));
                int x_out_left = xc - dxOuter;
                int x_out_right = xc + dxOuter;

                // Для внутренней окружности (если существует) также
                if (inR > 0 && dy * dy <= inR * inR)
                {
                    int dxInner = static_cast<int>(std::sqrt(inR * inR - dy * dy));
                    int x_in_left = xc - dxInner;
                    int x_in_right = xc + dxInner;

                    // Слева и справа от внутренней окружности
                    DrawHorizontalLine(y, x_out_left, x_in_left - 1, color);
                    DrawHorizontalLine(y, x_in_right + 1, x_out_right, color);
                } else
                {
                    // Если нет внутренней окружности -> вся линия
                    DrawHorizontalLine(y, x_out_left, x_out_right, color);
                }
            }

            // std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // m_window.display();
        }
    }

    void DrawHorizontalLine(int y, int x1, int x2, const Color &color)
    {
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++)
        {
            DrawPixel(x, y, color);
        }
    }

    void DrawThickCircle(Circle circle)
    {
        int xc = circle.GetPosition().m_x;
        int yc = circle.GetPosition().m_y;
        int radius = circle.GetRadius();

        int halfThickness = circle.GetOutThickness() / 2;
        int outR = radius + halfThickness;
        int inR = std::max(0, radius - halfThickness);

        FillBetweenCircles(xc, yc, inR, outR, circle.GetOutlineColor());

        // DrawCircleBresenham(xc, yc, outR, circle.GetOutlineColor());
        // DrawCircleBresenham(xc, yc, inR, circle.GetOutlineColor());
    }

public:
    Canvas(sf::RenderWindow &window)
        : m_window(window) {}

    void Draw(Circle circle)
    {
        DrawThickCircle(circle);
    }
};
