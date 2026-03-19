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
    constexpr  float PHI = (1.0f + sqrtf(5.0f)) / 2.0f;
    constexpr  float INV_PHI = 1.0f / PHI;

    static constexpr float vertices[20][3] = {
        // (±1, ±1, ±1)
        {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1}, // 0-3
        {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}, // 4-7
        // (0, ±1/phi, ±phi)
        {0, -INV_PHI, -PHI}, {0,  INV_PHI, -PHI}, {0, -INV_PHI,  PHI}, {0,  INV_PHI,  PHI}, // 8-11
        // (±1/phi, ±phi, 0)
        {-INV_PHI, -PHI, 0}, { INV_PHI, -PHI, 0}, {-INV_PHI,  PHI, 0}, { INV_PHI,  PHI, 0}, // 12-15
        // (±phi, 0, ±1/phi)
        {-PHI, 0, -INV_PHI}, { PHI, 0, -INV_PHI}, {-PHI, 0,  INV_PHI}, { PHI, 0,  INV_PHI}  // 16-19
    };

    static constexpr unsigned char faces[12][5] = {
        { 6, 15, 14,  7, 11}, // Верх
        { 4, 12, 13,  5, 10}, // Низ
        { 7, 14,  3, 16, 18}, // Левая сторона
        { 6, 11, 10,  5, 19}, // Передняя-правая
        { 7, 11, 10,  4, 18}, // Передняя-левая
        { 6, 19, 17,  2, 15}, // Правая-верхняя
        { 5, 19, 17,  1, 13}, // Правая-нижняя
        { 3, 16,  0,  8,  9}, // Задняя-левая
        { 2, 17,  1,  8,  9}, // Задняя-правая
        { 0, 16, 18,  4, 12}, // Нижняя-левая
        { 3, 14, 15,  2,  9}, // Верхняя-задняя
        { 0, 12, 13,  1,  8}  // Нижняя-задняя
    };
    static size_t const faceCount = sizeof(faces) / sizeof(*faces);

    glPushMatrix();
    glScalef(m_size * 0.5f, m_size * 0.5f, m_size * 0.5f);

    glBegin(GL_TRIANGLES);
    {
        for (int f = 0; f < 12; ++f) {
            // 1. Вычисляем центр грани (средняя точка 5 вершин)
            glm::vec3 center(0.0f);
            for (int i = 0; i < 5; ++i) {
                center += glm::make_vec3(vertices[faces[f][i]]);
            }
            center /= 5.0f;

            // 2. Вычисляем нормаль (для освещения)
            // Берем три точки звезды (например, центр и две вершины)
            glm::vec3 v1 = glm::make_vec3(vertices[faces[f][0]]);
            glm::vec3 v2 = glm::make_vec3(vertices[faces[f][1]]);
            glm::vec3 normal = glm::normalize(glm::cross(v1 - center, v2 - center));
            glNormal3fv(&normal[0]);

            // 3. Задаем цвет грани (из нашего массива цветов)
            glColor4ubv(m_sideColors[f]);

            // 4. Рисуем 5 треугольников, образующих пентаграмму
            for (int i = 0; i < 5; ++i) {
                int next = (i + 1) % 5;
                // Вершина 1: Центр звезды
                glVertex3fv(&center[0]);
                // Вершина 2: Текущий луч
                glVertex3fv(vertices[faces[f][i]]);
                // Вершина 3: Следующий луч (соединяем их «змейкой» для звезды)
                // ВАЖНО: для звездчатой формы соединяем i и (i+2)%5, чтобы получить перекрестия
                int starNext = (i + 2) % 5;
                glVertex3fv(vertices[faces[f][starNext]]);
            }
        }
    }
    glEnd();

    glPopMatrix();
}
