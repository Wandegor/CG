#pragma once

#include "Bezier.h"
#include "BezierRenderer.h"
#include <cmath>

class DragManager
{
public:
    DragManager(Bezier &bezier, BezierRenderer &renderer, float pickRadius = 0.04f)
            : m_bezier(bezier), m_renderer(renderer), m_pickRadius(pickRadius), m_selectedPoint(-1), m_dragging(false)
    {}

    void Update(float mx, float my, bool leftButtonPressed)
    {
        if (leftButtonPressed)
        {
            if (!m_dragging)
            {
                float bestDist = m_pickRadius;
                int bestIdx = -1;
                for (int i = 0; i < 4; ++i)
                {
                    float dx = mx - m_bezier.ControlPoints[i].x;
                    float dy = my - m_bezier.ControlPoints[i].y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < bestDist)
                    {
                        bestDist = dist;
                        bestIdx = i;
                    }
                }
                if (bestIdx != -1)
                {
                    m_selectedPoint = bestIdx;
                    m_dragging = true;
                }
            } else
            {
                if (m_selectedPoint != -1)
                {
                    m_bezier.ControlPoints[m_selectedPoint].x = mx;
                    m_bezier.ControlPoints[m_selectedPoint].y = my;

                    auto curvePoints = m_bezier.GenerateCurvePoints();
                    auto dashPoints = m_bezier.GenerateDashedLines();
                    m_renderer.UpdateCurve(curvePoints);
                    m_renderer.UpdateDashes(dashPoints);
                    m_renderer.UpdatePoints(m_bezier.ControlPoints, 4);
                }
            }
        } else
        {
            // сброс
            m_dragging = false;
            m_selectedPoint = -1;
        }
    }

private:
    Bezier &m_bezier;
    BezierRenderer &m_renderer;
    float m_pickRadius;
    int m_selectedPoint;
    bool m_dragging;
};