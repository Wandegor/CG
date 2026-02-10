#pragma once
#include "IJumpStrategy.h"

// задать начальную скорость в конструкторе

class UniformJump : public IJumpStrategy
{
private:
    float velocityY; // Скорость
    const float gravity;
    const float jumpForce;
    float groundLevel;

public:
    explicit UniformJump(
        float jumpForce,
        float gravity = 980.0f
    ): velocityY(-jumpForce),
       gravity(gravity),
       jumpForce(jumpForce),
       groundLevel(0) {}

    void StartJumping(float &y) override
    {
        // velocityY = -jumpForce;
        groundLevel = y;
    }

    void Update(float deltaTime, float& y) override
    {
        velocityY += gravity * deltaTime; // v = v0 + a*t
        y += velocityY * deltaTime; // S = S0 + v*t

        if (y >= groundLevel)
        {
            y = groundLevel;

            velocityY = -velocityY;
        }
    }
};
