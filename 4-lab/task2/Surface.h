#pragma once
#include "pch.h"
#include "Vertex.h"

class Surface
{
private:
    mutable GLuint m_displayList;

    int m_columns;
    int m_rows;

    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;

protected:
    virtual Vertex CalculateVertex(double x, double y) const
    {
        Vertex result =
        {
            {float(x), float(y), 0},
            {0, 1, 1, 1},
            {0, 0, 1}
        };
        return result;
    }

public:
    Surface(int columns, int rows, float xMin, float xMax, float yMin, float yMax)
        : m_displayList(0),
          m_columns(columns),
          m_rows(rows)
          , m_xMin(xMin)
          , m_xMax(xMax)
          , m_yMin(yMin)
          , m_yMax(yMax) {}

    ~Surface()
    {
        if (m_displayList!= 0)
        {
            glDeleteLists(m_displayList, 1);
        }
    }

    virtual void Draw() const
    {
        if (m_displayList == 0)
        {
            m_displayList = glGenLists(1);
            glNewList(m_displayList, GL_COMPILE);

            // вычисляем шаг узлов сетки
            const float dy = (m_yMax - m_yMin) / (m_rows - 1);
            const float dx = (m_xMax - m_xMin) / (m_columns - 1);

            float y = m_yMin;
            // пробегаем по строкам сетки
            for (int row = 0; row < m_rows - 1; ++row, y += dy)
            {
                // каждой строке будет соответствовать своя лента из треугольников
                glBegin(GL_TRIANGLE_STRIP);
                float x = m_xMin;

                // пробегаем по столбцам текущей строки
                for (int column = 0; column < m_columns; ++column, x += dx)
                {
                    // вычисляем параметры вершины в узлах пары соседних вершин
                    // ленты из треугольников
                    Vertex v0 = CalculateVertex(x, y + dy);
                    Vertex v1 = CalculateVertex(x, y);

                    glColor4f(v0.color.x, v0.color.y, v0.color.z, v0.color.w);
                    // задаем нормаль и координаты вершины на четной позиции
                    glNormal3f(v0.normal.x, v0.normal.y, v0.normal.z);
                    glVertex3f(v0.position.x, v0.position.y, v0.position.z);

                    glColor4f(0.5, 0, v0.color.z, v0.color.w);
                    // задаем нормаль и координаты вершины на нечетной позиции
                    glNormal3f(v1.normal.x, v1.normal.y, v1.normal.z);
                    glVertex3f(v1.position.x, v1.position.y, v1.position.z);
                }
                glEnd();
            }

            glEndList();
        }

        // Вызовем ранее записанный дисплейный список команд рисования сетки
        glCallList(m_displayList);
    }
};
