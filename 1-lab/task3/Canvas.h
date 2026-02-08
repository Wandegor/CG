#pragma once

#include <complex>
#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Circle.h"


class Canvas
{
    sf::RenderWindow &m_window;
    sf::Color m_backgroundColor = sf::Color::Black;

    void DrawPixel(float fx, float fy, Color color, float alpha = 1.0f)
    {
        int x = static_cast<int>(fx);
        int y = static_cast<int>(fy);

        if (x >= 0 && x < static_cast<int>(m_window.getSize().x) &&
            y >= 0 && y < static_cast<int>(m_window.getSize().y))
        {
            sf::Color blended(
                (color.r * alpha + m_backgroundColor.r * (1 - alpha)),
                (color.g * alpha + m_backgroundColor.g * (1 - alpha)),
                (color.b * alpha + m_backgroundColor.b * (1 - alpha))
            );

            sf::Vertex point(sf::Vector2f(x, y), blended);
            m_window.draw(&point, 1, sf::PrimitiveType::Points);
        }
    }

    void FillBetweenCircles(int xc, int yc, int inR, int outR, Color outlineColor, Color fillColor)
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

                    // Внутренность круга
                    DrawHorizontalLine(y, x_in_left, x_in_right, fillColor);

                    // Слева и справа от внутренней окружности
                    DrawHorizontalLine(y, x_out_left, x_in_left - 1, outlineColor);
                    DrawHorizontalLine(y, x_in_right + 1, x_out_right, outlineColor);
                } else
                {
                    // Если нет внутренней окружности -> вся линия
                    DrawHorizontalLine(y, x_out_left, x_out_right, outlineColor);
                }
            }

            // std::this_thread::sleep_for(std::chrono::milliseconds(0));
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

    void DrawOuterCircleWu(int xc, int yc, int outR, const Color& color)
    {
        for (int x = 0; x <= outR * 707 / 1000; x++) // x > R/√2 координаты начинают повторяться
        {
            float y_exact = std::sqrt(outR * outR - x * x);
            int y = static_cast<int>(y_exact);
            float fraction = y_exact - y;

            float intensity1 = 1.0f - fraction; // основной
            float intensity2 = fraction; // дополнительный

            DrawPixel(xc + x, yc + y, color, intensity1);
            DrawPixel(xc + x, yc + y + 1, color, intensity2);

            // Второй октант: (y, x)
            DrawPixel(xc + y, yc + x, color, intensity1);
            DrawPixel(xc + y + 1, yc + x, color, intensity2);

            // Третий октант: (-x, y)
            DrawPixel(xc - x, yc + y, color, intensity1);
            DrawPixel(xc - x, yc + y + 1, color, intensity2);

            // Четвертый октант: (-y, x)
            DrawPixel(xc - y, yc + x, color, intensity1);
            DrawPixel(xc - y - 1, yc + x, color, intensity2);

            // Пятый октант: (x, -y)
            DrawPixel(xc + x, yc - y, color, intensity1);
            DrawPixel(xc + x, yc - y - 1, color, intensity2);

            // Шестой октант: (y, -x)
            DrawPixel(xc + y, yc - x, color, intensity1);
            DrawPixel(xc + y + 1, yc - x, color, intensity2);

            // Седьмой октант: (-x, -y)
            DrawPixel(xc - x, yc - y, color, intensity1);
            DrawPixel(xc - x, yc - y - 1, color, intensity2);

            // Восьмой октант: (-y, -x)
            DrawPixel(xc - y, yc - x, color, intensity1);
            DrawPixel(xc - y - 1, yc - x, color, intensity2);

            // std::this_thread::sleep_for(std::chrono::milliseconds(5));
            // m_window.display();
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


        DrawOuterCircleWu(xc, yc, outR, circle.GetOutlineColor());

        FillBetweenCircles(xc, yc, inR, outR, circle.GetOutlineColor(), circle.GetFillColor());
    }

public:
    Canvas(sf::RenderWindow &window)
        : m_window(window) {}

    void Draw(Circle circle)
    {
        DrawThickCircle(circle);
    }
};
