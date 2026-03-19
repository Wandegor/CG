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
    if (faceIndex >= 0 && faceIndex < 12)
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
    constexpr float phi = (1.0f + sqrtf(5.0f)) / 2.0f; // ~1.61803
    constexpr float spike_factor = phi * phi; // ~2.61803 (phi^2)

    const float s = m_size;
    // База: 12 уникальных вершин Икосаэдра
    const glm::vec3 verts[12] = {
        {-s, s * phi, 0}, {s, s * phi, 0}, {-s, -s * phi, 0}, {s, -s * phi, 0},
        {0, -s, s * phi}, {0, s, s * phi}, {0, -s, -s * phi}, {0, s, -s * phi},
        {s * phi, 0, -s}, {s * phi, 0, s}, {-s * phi, 0, -s}, {-s * phi, 0, s}
    };

    // База: 20 граней Икосаэдра (индексы вершин)
    static constexpr int faces[20][3] = {
        {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
        {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
        {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
        {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
    };

    std::vector<glm::vec3> all_vertices;
    all_vertices.reserve(32);

    // Сначала добавляем 12 вершин икосаэдра
    for (int i = 0; i < 12; ++i) all_vertices.push_back(verts[i]);

    // Затем добавляем 20 вершин (peaks) шипов
    for (int i = 0; i < 20; ++i)
    {
        glm::vec3 v0 = verts[faces[i][0]];
        glm::vec3 v1 = verts[faces[i][1]];
        glm::vec3 v2 = verts[faces[i][2]];
        glm::vec3 center = (v0 + v1 + v2) / 3.0f;
        all_vertices.push_back(center * spike_factor); // Вершины spikes будут иметь индексы 12-31
    }

    std::vector<unsigned int> indices;
    indices.reserve(180); // 60 треугольников * 3 вершины

    glPushMatrix();
    // Уменьшение масштаба
    glScalef(m_size * 0.3f, m_size * 0.3f, m_size * 0.3f);

    // Проходим по каждому шипу
    for (int i = 0; i < 20; ++i)
    {
        // Индексы вершин основания (0-11)
        int v0_idx = faces[i][0];
        int v1_idx = faces[i][1];
        int v2_idx = faces[i][2];
        // Индекс вершины peak (12-31)
        int peak_idx = 12 + i;

        // Добавляем 3 треугольника, образующих боковые стенки шипа
        // Треугольник 1 (v0, v1, peak)
        indices.push_back(v0_idx);
        indices.push_back(v1_idx);
        indices.push_back(peak_idx);
        // Треугольник 2 (v1, v2, peak)
        indices.push_back(v1_idx);
        indices.push_back(v2_idx);
        indices.push_back(peak_idx);
        // Треугольник 3 (v2, v0, peak)
        indices.push_back(v2_idx);
        indices.push_back(v0_idx);
        indices.push_back(peak_idx);
    }

    // Включаем смещение для граней
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    {
        // Проходим по всем 60 треугольникам (каждые 3 индекса)
        for (size_t i = 0; i < indices.size() / 3; ++i)
        {
            // Красим шипы (каждые 3 треугольника — один шип)
            if (i % 3 == 0)
            {
                glColor4ubv(m_sideColors[(i / 3) % 12]);
            }
            // Получаем 3 вершины треугольника по их индексам
            const glm::vec3& v0 = all_vertices[indices[i * 3 + 0]];
            const glm::vec3& v1 = all_vertices[indices[i * 3 + 1]];
            const glm::vec3& peak = all_vertices[indices[i * 3 + 2]];

            // Расчет нормали для Fill (свет)
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

    glBegin(GL_TRIANGLES);
    {
        // Рисуем ту же самую последовательность индексов!
        for (unsigned int idx: indices)
        {
            glVertex3fv(&all_vertices[idx][0]);
        }
    }
    glEnd();

    // Восстанавливаем состояние
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glEnable(GL_LIGHTING);
}
