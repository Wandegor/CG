#pragma once
#include <vector>
#include <glad/gl.h>

class RenderObject
{
public:
    RenderObject(const std::vector<float>& vertices, int vertexSize = 3)
    {
        m_vertexCount = static_cast<GLsizei>(vertices.size() / vertexSize);

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        int stride = vertexSize * sizeof(float);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(0);

        if (vertexSize == 5)
        {
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

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
