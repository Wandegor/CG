#pragma once
#include <vector>
#include <glad/gl.h>

class RenderObject
{
public:
    RenderObject(const std::vector<float>& vertices)
    {
        m_vertexCount = static_cast<GLsizei>(vertices.size() / 3);

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~RenderObject()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void Draw(GLenum mode = GL_TRIANGLES) const
    {
        glBindVertexArray(m_vao);
        glDrawArrays(mode, 0, m_vertexCount);
        glBindVertexArray(0);
    }

private:
    GLuint m_vao = 0, m_vbo = 0;
    GLsizei m_vertexCount = 0;
};
