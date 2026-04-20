#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
    }

    Mesh() = default;

    void Draw()
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // sizeof(Vertex) - это шаг (stride), чтобы OpenGL знал, через сколько байт следующая точка
        glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertices[0].Position);
        glNormalPointer(GL_FLOAT, sizeof(Vertex), &vertices[0].Normal);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].TexCoords);

        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, &indices[0]);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
};
