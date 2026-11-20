#pragma once
#include <vector>
#include <cmath>

class ShapeFactory
{
public:
    static std::vector<float> CreateTriangle()
    {
        return {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
        };
    }

    static std::vector<float> CreateCanabola(int pointsCount = 2000)
    {
        std::vector<float> vertices;
        for (int i = 0; i <= pointsCount; ++i)
        {
            float x = (2.0f * (float)M_PI * (float)i) / (float)pointsCount;
            float R = (1.0f + sin(x)) * (1.0f + 0.9f * cos(8.0f * x)) * (1.0f + 0.1f * cos(24.0f * x)) * (
                          0.5f + 0.05f * cos(140.0f * x));

            vertices.push_back(R * cos(x));
            vertices.push_back(R * sin(x));
            vertices.push_back(0.0f);
        }
        return vertices;
    }
};
