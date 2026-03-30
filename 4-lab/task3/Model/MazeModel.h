#pragma once
#include "../pch.h"

struct MazeCell
{
    bool isWall = false;
    glm::u8vec4 color{255, 255, 255, 255}; // Цвет стен (RGBA)
};

class MazeModel
{
private:
    static constexpr int MAZE_SIZE = 16;
    static constexpr double PLAYER_HEIGHT = 0.5; // Высота камеры

    std::array<std::array<MazeCell, MAZE_SIZE>, MAZE_SIZE> m_grid;
    glm::dvec3 m_playerPosition{};
    double m_playerYaw; // Угол поворота влево-вправо

    [[nodiscard]] bool IsCollision(double x, double z) const
    {
        int ix = static_cast<int>(std::floor(x));
        int iz = static_cast<int>(std::floor(z));

        // Если вышли за пределы массива или попали в ячейку со стеной
        if (ix < 0 || ix >= MAZE_SIZE || iz < 0 || iz >= MAZE_SIZE) return true;

        return m_grid[ix][iz].isWall;
    }

public:
    MazeModel()
    {
        for(int x = 0; x < 10; ++x) {
            m_grid[x][5] = {true, {255, 0, 0, 255}}; // Стена на Z = 5
        }

        // Ставим игрока в Z = 10.0 (стена теперь перед ним, в Z=5)
        m_playerPosition = {5.0, PLAYER_HEIGHT, 10.0};
        m_playerYaw = 0.0; // Смотрит в сторону -Z (как раз на стену)
    }

    [[nodiscard]] const MazeCell& GetCell(int x, int z) const
    {
        if (x >= 0 && x < MAZE_SIZE && z >= 0 && z < MAZE_SIZE)
        {
            return m_grid[x][z];
        }
        static MazeCell boundaryWall = {true, {0, 0, 0, 255}};
        return boundaryWall;
    }

    [[nodiscard]] glm::dvec3 GetPosition() const { return m_playerPosition; }
    [[nodiscard]] double GetYaw() const { return m_playerYaw; }

    void Rotate(double angleDelta)
    {
        m_playerYaw += angleDelta;
    }

    void Move(double distanceForward, double distanceRight)
    {
        double sinYaw = std::sin(m_playerYaw);
        double cosYaw = std::cos(m_playerYaw);

        glm::dvec3 forwardDir = {sinYaw, 0.0, -cosYaw};
        glm::dvec3 rightDir = {cosYaw, 0.0, sinYaw};

        glm::dvec3 nextPos = m_playerPosition
                             + forwardDir * distanceForward
                             + rightDir * distanceRight;

        // Простая проверка коллизий (по отдельным осям для плавного скольжения вдоль стен)
        if (!IsCollision(nextPos.x, m_playerPosition.z))
        {
            m_playerPosition.x = nextPos.x;
        }
        if (!IsCollision(m_playerPosition.x, nextPos.z))
        {
            m_playerPosition.z = nextPos.z;
        }
    }
};
