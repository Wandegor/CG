#pragma once
class StarPolyhedron
{
public:
    explicit StarPolyhedron(float size = 1);
    void SetSideColor(int faceIndex, GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);

    void DrawEdges() const;
    void DrawFaces(const glm::dmat4& cameraMatrix) const;

private:
    void GenerateGeometry();

    float m_size;

    GLubyte m_sideColors[20][4]{};

    std::vector<glm::vec3> m_vertices;
    std::vector<GLuint> m_indices;

    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    float spike_factor = phi * phi;
};
