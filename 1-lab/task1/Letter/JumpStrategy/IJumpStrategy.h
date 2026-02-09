#pragma once
class Letter;

class IJumpStrategy
{
public:
    virtual ~IJumpStrategy() = default;

    virtual void StartJumping(float &y) = 0;
    virtual void Update(float deltaTime, float &y) = 0;
};
