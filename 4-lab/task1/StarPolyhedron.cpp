#include "pch.h"
#include "StarPolyhedron.h"

StarPolyhedron::StarPolyhedron(float size)
    : m_size(size)
{
    for (int i = 0; i < 12; ++i)
    {
        SetSideColor(i, 255, 255, 255);
    }
}

void StarPolyhedron::SetSideColor(int faceIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    // Проверка, чтобы не вылететь за пределы массива
    if (faceIndex >= 0 && faceIndex < 12)
    {
        m_sideColors[faceIndex][0] = r;
        m_sideColors[faceIndex][1] = g;
        m_sideColors[faceIndex][2] = b;
        m_sideColors[faceIndex][3] = a;
    }
}

void StarPolyhedron::Draw() const
{
    // Золотое сечение
    constexpr float phi = (1.0f + sqrtf(5.0f)) / 2.0f;

    // 1. База: 12 вершин Икосаэдра
    static constexpr float vert[12][3] = {
        {-1,  phi, 0}, { 1,  phi, 0}, {-1, -phi, 0}, { 1, -phi, 0},
        {0, -1,  phi}, {0,  1,  phi}, {0, -1, -phi}, {0,  1, -phi},
        { phi, 0, -1}, { phi, 0,  1}, {-phi, 0, -1}, {-phi, 0,  1}
    };

    // 2. База: 20 граней Икосаэдра (индексы вершин)
    static constexpr int faces[20][3] = {
        {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
        {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
        {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
        {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };

    glPushMatrix();
    // Немного уменьшим масштаб, так как шипы сильно выдаются вперед
    glScalef(m_size * 0.3f, m_size * 0.3f, m_size * 0.3f);

    glBegin(GL_TRIANGLES);
    {
        // Проходим по всем 20 треугольникам базового икосаэдра
        for (int i = 0; i < 20; ++i) {

            // Получаем 3 вершины текущего треугольника
            glm::vec3 v0 = glm::make_vec3(vert[faces[i][0]]);
            glm::vec3 v1 = glm::make_vec3(vert[faces[i][1]]);
            glm::vec3 v2 = glm::make_vec3(vert[faces[i][2]]);

            // Находим центр этого треугольника
            glm::vec3 center = (v0 + v1 + v2) / 3.0f;

            // МАГИЯ: Вытягиваем центр наружу, чтобы получить острие шипа
            // Коэффициент 3/phi математически точно формирует Большой звездчатый додекаэдр
            glm::vec3 peak = center * phi * phi;

            // Красим каждый шип. Используем остаток от деления, чтобы уложиться в 12 твоих цветов
            glColor4ubv(m_sideColors[i % 12]);

            // Теперь рисуем 3 треугольника, которые образуют боковые стенки этого шипа (пирамиды)

            // Стенка 1 (v0, v1, peak)
            glm::vec3 n1 = glm::normalize(glm::cross(v1 - v0, peak - v0));
            glNormal3fv(&n1[0]);
            glVertex3fv(&v0[0]); glVertex3fv(&v1[0]); glVertex3fv(&peak[0]);

            // Стенка 2 (v1, v2, peak)
            glm::vec3 n2 = glm::normalize(glm::cross(v2 - v1, peak - v1));
            glNormal3fv(&n2[0]);
            glVertex3fv(&v1[0]); glVertex3fv(&v2[0]); glVertex3fv(&peak[0]);

            // Стенка 3 (v2, v0, peak)
            glm::vec3 n3 = glm::normalize(glm::cross(v0 - v2, peak - v2));
            glNormal3fv(&n3[0]);
            glVertex3fv(&v2[0]); glVertex3fv(&v0[0]); glVertex3fv(&peak[0]);
        }
    }
    glEnd();

    glPopMatrix();
}
