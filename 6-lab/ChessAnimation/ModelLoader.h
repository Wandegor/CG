#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class ModelLoader
{
private:
    std::map<std::string, Mesh> m_meshes;

public:

    ModelLoader(const std::string& path)
    {
        Load(path);
    }

    void DrawMesh(const std::string& name) {
        if (m_meshes.count(name)) {
            m_meshes[name].Draw();
        }
    }

private:
    void Load(const std::string& path)
    {
        Assimp::Importer importer;

        // 1. Triangulate - превратить всё в треугольники
        // 2. FlipUVs - перевернуть текстуры под стандарт OpenGL
        // 3. GenNormals - создать нормали, если их забыл добавить автор модели
        const aiScene* scene = importer.ReadFile(path,
                                                 aiProcess_Triangulate | aiProcess_FlipUVs |
                                                 aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "Error Assimp: " << importer.GetErrorString() << std::endl;
            return;
        }

        // Начинаем рекурсивно обходить дерево объектов модели
        ProcessNode(scene->mRootNode, scene);
    }

    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        // Обрабатываем все меши в текущем узле
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            std::string name = node->mName.C_Str();
            std::cout << "Model Name: [" << name << "]" << std::endl;
            m_meshes[name] = ProcessMesh(mesh, scene);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Координаты вершин, нормали, коорд текстур
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            if (mesh->HasNormals())
            {
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // индексы (порядок соединения вершин в треугольники)
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, indices);
    }
};
