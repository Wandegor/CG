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
};
