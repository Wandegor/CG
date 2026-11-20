#pragma once
#include <vector>
#include <cmath>

struct MeshData {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class ShapeFactory
{
public:
    static MeshData CreateMesh(int resolution = 50)
    {
        MeshData mesh;

        for (int i = 0; i <= resolution; ++i)
        {
            for (int j = 0; j <= resolution; ++j)
            {
                mesh.vertices.push_back((float)i / resolution); // u
                mesh.vertices.push_back((float)j / resolution); // v
                mesh.vertices.push_back(0.0f);
            }
        }

        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                int row1 = i * (resolution + 1);
                int row2 = (i + 1) * (resolution + 1);

                // Два треугольника для каждого квадрата сетки
                mesh.indices.push_back(row1 + j);
                mesh.indices.push_back(row1 + j + 1);
                mesh.indices.push_back(row2 + j);

                mesh.indices.push_back(row1 + j + 1);
                mesh.indices.push_back(row2 + j + 1);
                mesh.indices.push_back(row2 + j);
            }
        }
        return mesh;
    }
};
