#pragma once
#include <vector>
#include <cmath>

struct MeshData
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class ShapeFactory
{
public:
    static MeshData CreateMesh()
    {
        MeshData mesh;

        mesh.vertices = {
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // Лев. верх
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Лев. низ
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Прав. низ
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f  // Прав. верх
        };
        mesh.indices = { 0, 1, 2, 0, 2, 3 };

        return mesh;
    }
};
