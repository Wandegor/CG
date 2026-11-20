#pragma once
#include <cmath>

struct Mat3
{
    float data[9];

    static Mat3 translation(float tx, float ty)
    {
        return Mat3{1, 0, tx,
                    0, 1, ty,
                    0, 0, 1};
    }

    static Mat3 rotation(float angle)
    {
        float cos = std::cos(angle);
        float sin = std::sin(angle);

        return Mat3{cos, -sin, 0,
                    sin, cos, 0,
                    0, 0, 1};
    }

    static Mat3 scale(float sx, float sy)
    {
        return Mat3{sx, 0, 0,
                    0, sy, 0,
                    0, 0, 1};
    }

    Mat3 operator*(const Mat3 &other) const
    {
        Mat3 result{};

        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                result.data[row * 3 + col] =
                        data[row * 3 + 0] * other.data[0 * 3 + col] +
                        data[row * 3 + 1] * other.data[1 * 3 + col] +
                        data[row * 3 + 2] * other.data[2 * 3 + col];
            }
        }

        return result;
    }
};
