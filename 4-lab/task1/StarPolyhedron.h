#pragma once
class StarPolyhedron
{
public:
    explicit StarPolyhedron(float size = 1);
    void Draw()const;
    void SetSideColor(int faceIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);

private:
    void GenerateGeometry();
    float m_size;

    GLubyte m_sideColors[20][4]{};

    std::vector<glm::vec3> m_vertices;
    std::vector<GLuint> m_indices;
};
