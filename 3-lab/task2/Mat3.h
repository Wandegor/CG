#include <cmath>

#pragma once
struct Mat3
{
    float data[9];

    static Mat3 identity()
    {
        return Mat3{1, 0, 0, 0, 1, 0, 0, 0, 1};
    }

    static Mat3 translation(float tx, float ty)
    {
        return Mat3{1, 0, tx, 0, 1, ty, 0, 0, 1};
    }

    static Mat3 rotation(float angle)
    {
        Mat3 r{};

        float c = std::cos(angle);
        float s = std::sin(angle);

        r.data[0] = c;
        r.data[1] = -s;
        r.data[2] = 0.0f;

        r.data[3] = s;
        r.data[4] = c;
        r.data[5] = 0.0f;

        r.data[6] = 0.0f;
        r.data[7] = 0.0f;
        r.data[8] = 1.0f;

        return r;
    }
    Mat3 operator*(const Mat3& other) const
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
