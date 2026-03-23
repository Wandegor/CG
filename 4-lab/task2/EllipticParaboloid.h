#pragma once
#include "Surface.h"

class EllipticParaboloid : public Surface
{
private:
    float m_size;
public:
    explicit EllipticParaboloid(float size = 1)
    {}
    void Draw() const;
};
