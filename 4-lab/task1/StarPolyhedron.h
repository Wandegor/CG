#pragma once
class StarPolyhedron
{
public:
    explicit StarPolyhedron(float size = 1);

    void Draw()const;

    void SetSideColor(int faceIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);
private:
    float m_size;

    // Цвета сторон куба
    GLubyte m_sideColors[12][4]{};
};
