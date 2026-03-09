#pragma once
#include <vector>

#include "Common/Point.h"

class Bezier
{
private:
    const int SEGMENTS = 10;
    const float POINT_SIZE = 10.0f; // размер контрольных точек

    const int DASH_COUNT = 20; // количество штрихов на один отрезок
    const float DASH_LENGTH = 0.5f; // доля отрезка, занятая штрихом (0..1)

public:
    // Контрольные точки кубической кривой Безье
    Point ControlPoints[4] = {
        {-0.7f, -0.5f},
        {-0.3f, 0.7f},
        {0.5f, 0.7f},
        {0.7f, -0.5f}
    };

    // Вычисление точки на кривой Безье при параметре t (0..1)
    Point BezierPoint(float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        return {
            uuu * ControlPoints[0].x + 3.0f * uu * t * ControlPoints[1].x + 3.0f * u * tt * ControlPoints[2].x + ttt *
            ControlPoints[3].x,
            uuu * ControlPoints[0].y + 3.0f * uu * t * ControlPoints[1].y + 3.0f * u * tt * ControlPoints[2].y + ttt *
            ControlPoints[3].y
        };
    }

    // Генерация точек кривой (ломаная линия)
    std::vector<Point> GenerateCurvePoints()
    {
        std::vector<Point> points;
        for (int i = 0; i <= SEGMENTS; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(SEGMENTS);
            points.push_back(BezierPoint(t));
        }
        return points;
    }

    // Генерация пунктирных линий между контрольными точками
    std::vector<Point> GenerateDashedLines()
    {
        std::vector<Point> vertices;


        // Для каждой пары контрольных точек
        for (int pair = 0; pair < 3; ++pair)
        {
            Point p1 = ControlPoints[pair];
            Point p2 = ControlPoints[pair + 1];

            for (int i = 0; i < DASH_COUNT; ++i)
            {
                float start = static_cast<float>(i) / DASH_COUNT;
                float end = start + DASH_LENGTH / DASH_COUNT; // длина штриха в параметрическом пространстве

                // Если конец штриха выходит за пределы, обрезаем
                if (end > 1.0f) end = 1.0f;

                // Линейная интерполяция
                Point a = {p1.x + start * (p2.x - p1.x), p1.y + start * (p2.y - p1.y)};
                Point b = {p1.x + end * (p2.x - p1.x), p1.y + end * (p2.y - p1.y)};

                vertices.push_back(a);
                vertices.push_back(b);
            }
        }
        return vertices;
    }

    float GetPointSize() const { return POINT_SIZE; }
};
