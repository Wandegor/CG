#pragma once
#include <vector>

class Model
{
private:
    int m_width;
    int m_height;
    std::vector<std::vector<bool>> m_grid;

public:
    Model(int width, int height) : m_width(width), m_height(height)
    {
        m_grid.resize(width, std::vector<bool>(height, false));
        int map[4][4] = {
            {1,1,1,1},
            {1,0,0,0},
            {1,0,1,1},
            {1,0,1,0},
        };

        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                if (map[z][x] == 1) {
                    m_grid[x][z] = true;
                }
            }
        }
    }

    [[nodiscard]] int GetWidth() const { return m_width; }
    [[nodiscard]] int GetHeight() const { return m_height; }
};