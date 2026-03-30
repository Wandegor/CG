#pragma once
#include <vector>

class MazeModel
{
private:
    int m_width;
    int m_height;
    std::vector<std::vector<bool>> m_grid;

public:
    MazeModel(int width, int height) : m_width(width), m_height(height)
    {
        m_grid.resize(width, std::vector<bool>(height, false));
        int map[10][10] = {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,1,0,0,0,0,1},
            {1,0,1,0,1,0,1,1,0,1},
            {1,0,1,0,0,0,0,1,0,1},
            {1,0,1,1,1,1,0,1,0,1},
            {1,0,0,0,0,1,0,0,0,1},
            {1,1,1,1,0,1,1,1,0,1},
            {1,0,0,0,0,0,0,1,0,1},
            {1,0,1,1,1,1,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        };

        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                // Если в массиве 1 — ставим стену
                if (map[z][x] == 1) {
                    m_grid[x][z] = true;
                }
            }
        }
    }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    bool IsWall(int x, int z) const
    {
        if (x < 0 || x >= m_width || z < 0 || z >= m_height) return true;
        return m_grid[x][z];
    }
};