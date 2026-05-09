#pragma once
#include <vector>
#include <glad/gl.h>

class RenderObject
{
public:
    RenderObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, int vertexSize = 8)
    {
        m_indexCount = static_cast<GLsizei>(indices.size());

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        GLsizei stride = vertexSize * sizeof(float);

        // 3 Pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
        glEnableVertexAttribArray(0);

        if (vertexSize == 6)
        {
            // 3 Nor
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        if (vertexSize == 8)
        {
            // 3 Nor
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // 2 Tex
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        glBindVertexArray(0);
    }

    ~RenderObject()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void Draw(GLenum mode = GL_TRIANGLES) const
    {
        glBindVertexArray(m_vao);
        glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
    GLsizei m_indexCount;
};
