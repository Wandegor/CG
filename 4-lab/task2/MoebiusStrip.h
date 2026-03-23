#pragma once
#include "Surface.h"

class MoebiusStrip : public Surface
{
private:
    float m_size;

protected:
    Vertex CalculateVertex(double u, double v) const override
    {
        const double cu = cos(u);
        const double su = sin(u);
        const double cu2 = cos(u / 2.0);
        const double su2 = sin(u / 2.0);

        const double a = 1.0 + (v / 2.0) * cu2;

        const double x = a * cu;
        const double y = a * su;
        const double z = (v / 2.0) * su2;

        // Нормали
        const double dxdu = -(v / 4.0) * su2 * cu - a * su;
        const double dydu = -(v / 4.0) * su2 * su + a * cu;
        const double dzdu =  (v / 4.0) * cu2;

        const double dxdv = 0.5 * cu2 * cu;
        const double dydv = 0.5 * cu2 * su;
        const double dzdv = 0.5 * su2;

        glm::dvec3 du(dxdu, dydu, dzdu);
        glm::dvec3 dv(dxdv, dydv, dzdv);

        glm::dvec3 n = glm::normalize(glm::cross(du, dv));

        return Vertex
        {
            {float(x), float(y), float(z)},
            {0, 1, 1, 1},
            {float(n.x), float(n.y), float(n.z)}
        };
    }

public:
    explicit MoebiusStrip(int columns, int rows)
        : Surface(columns, rows, 0.f, float(2.0 * M_PI), -1.0f, 1.0f) {}
};
