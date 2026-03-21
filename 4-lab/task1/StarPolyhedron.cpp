#include "pch.h"
#include "StarPolyhedron.h"

StarPolyhedron::StarPolyhedron(float size)
    : m_size(size)
{
    for (int i = 0; i < 20; ++i)
    {
        SetSideColor(i, 255, 255, 255);
    }

    GenerateGeometry();
}

void StarPolyhedron::GenerateGeometry()
{
    // 12 вершин Икосаэдра
    const glm::vec3 base_verts[12] = {
        {-1, phi, 0}, {1, phi, 0}, {-1, -phi, 0}, {1, -phi, 0},
        {0, -1, phi}, {0, 1, phi}, {0, -1, -phi}, {0, 1, -phi},
        {phi, 0, -1}, {phi, 0, 1}, {-phi, 0, -1}, {-phi, 0, 1}
    };

    // 20 граней Икосаэдра (основание пирамид)
    static constexpr int faces[20][3] = {
        {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
        {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
        {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
        {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };

    m_vertices.clear();
    m_indices.clear();

    // Сохраняем вершины икосаэдра
    for (int i = 0; i < 12; ++i)
        m_vertices.push_back(base_verts[i] * m_size);

    // Считаем пики шипов
    for (int i = 0; i < 20; ++i)
    {
        glm::vec3 v0 = base_verts[faces[i][0]];
        glm::vec3 v1 = base_verts[faces[i][1]];
        glm::vec3 v2 = base_verts[faces[i][2]];
        glm::vec3 center = (v0 + v1 + v2) / 3.0f; // пик пирамиды
        m_vertices.push_back(center * spike_factor * m_size);

        // Заполняем индексы сразу
        int peak_idx = 12 + i;
        m_indices.push_back(faces[i][0]);
        m_indices.push_back(faces[i][1]);
        m_indices.push_back(peak_idx);
        m_indices.push_back(faces[i][1]);
        m_indices.push_back(faces[i][2]);
        m_indices.push_back(peak_idx);
        m_indices.push_back(faces[i][2]);
        m_indices.push_back(faces[i][0]);
        m_indices.push_back(peak_idx);
    }
}

void StarPolyhedron::SetSideColor(int faceIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    if (faceIndex >= 0 && faceIndex < 20)
    {
        m_sideColors[faceIndex][0] = r;
        m_sideColors[faceIndex][1] = g;
        m_sideColors[faceIndex][2] = b;
        m_sideColors[faceIndex][3] = a;
    }
}

#include <vector>

void StarPolyhedron::Draw() const
{
    // Включаем смещение для граней
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glPushMatrix();
    // Уменьшение масштаба
    glScalef(m_size * 0.3f, m_size * 0.3f, m_size * 0.3f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES); {
        // Проходим по всем 60 треугольникам (каждые 3 индекса)
        for (size_t i = 0; i < m_indices.size() / 3; ++i)
        {
            // Красим шипы (каждые 3 треугольника — один шип)
            if (i % 3 == 0)
            {
                glColor4ubv(m_sideColors[(i / 3) % 20]);
            }
            // Получаем 3 вершины треугольника по их индексам
            const glm::vec3& v0 = m_vertices[m_indices[i * 3 + 0]];
            const glm::vec3& v1 = m_vertices[m_indices[i * 3 + 1]];
            const glm::vec3& peak = m_vertices[m_indices[i * 3 + 2]];

            // Расчет нормали света
            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, peak - v0));
            glNormal3fv(&normal[0]);

            glVertex3fv(&v0[0]);
            glVertex3fv(&v1[0]);
            glVertex3fv(&peak[0]);
        }
    }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    // ребра
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    // glDisable(GL_LIGHTING); // Важно для четких линий

    glBegin(GL_TRIANGLES); {
        // Рисуем ту же самую последовательность индексов!
        for (unsigned int idx: m_indices)
        {
            glVertex3fv(&m_vertices[idx][0]);
        }
    }
    glEnd();

    glEnable(GL_LIGHTING);
    // Восстанавливаем состояние
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glEnable(GL_LIGHTING);
    glPopMatrix();
}
