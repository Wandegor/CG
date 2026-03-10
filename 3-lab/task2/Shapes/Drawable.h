#pragma once
#include <glad/glad.h>
#include <vector>
#include "../Common/Point.h"

class Drawable
{
protected:
    GLuint VAO, VBO;
    size_t vertexCount;
    GLenum primitiveType;

public:
    Drawable(GLenum primitive = GL_TRIANGLES) : VAO(0), VBO(0), vertexCount(0), primitiveType(primitive) {}

    virtual ~Drawable()
    {
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
    }

    void Create(const std::vector<Point>& vertices)
    {
        vertexCount = vertices.size();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Point), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw() const
    {
        glBindVertexArray(VAO);
        glDrawArrays(primitiveType, 0, vertexCount);
    }
};
