#pragma once
#include <vector>

#include "IModelListener.h"

struct Tile
{
    int id;
    bool isOpen = false;
    bool isRemoved = false;
};

class Model
{
private:
    int m_rows;
    int m_cols;
    std::vector<std::vector<Tile> > m_grid;

    int m_firstRow = -1, m_firstCol = -1;

    std::vector<IModelListener *> m_listeners;

public:
    Model(int rows, int cols) : m_rows(rows), m_cols(cols)
    {
        Reset();
    }

    void Reset()
    {
        m_grid.assign(m_rows, std::vector<Tile>(m_cols));

        // все карты
        std::vector<int> ids;

        int numPairs = m_rows * m_cols / 2;
        for (int i = 0; i < numPairs; i++)
        {
            ids.push_back(i);
            ids.push_back(i);
        }

        std::random_shuffle(ids.begin(), ids.end());

        int index = 0;
        for (int r = 0; r < m_rows; r++)
        {
            for (int c = 0; c < m_cols; c++)
            {
                m_grid[r][c].id = ids[index++];
                m_grid[r][c].isOpen = false;
                m_grid[r][c].isRemoved = false;
            }
        }
    }

    void PressTile(int r, int c)
    {
        if (m_grid[r][c].isRemoved) return; // уже удалена - выход
        if (m_grid[r][c].isOpen) // уже открыта - закрыть
        {
            m_grid[r][c].isOpen = false;

            m_firstRow = -1;
            m_firstCol = -1;
            NotifyListeners();
            return;
        }

        // закрыта - открываем, проверка на пару
        m_grid[r][c].isOpen = true;

        if (m_firstRow == -1)
        {
            // Это первая открытая карточка
            m_firstRow = r;
            m_firstCol = c;
        }
        else
        {
            // Это вторая. Сравниваю с id первой
            if (m_grid[r][c].id == m_grid[m_firstRow][m_firstCol].id)
            {
                // Совпали - Пока просто остаются открытыми
                m_grid[r][c].isRemoved = true;
                m_grid[m_firstRow][m_firstCol].isRemoved = true;
            }
            else
            {
                // Не совпали - закрыть
                m_grid[r][c].isOpen = false;
                m_grid[m_firstRow][m_firstCol].isOpen = false;
            }

            m_firstRow = -1;
            m_firstCol = -1;
        }

        NotifyListeners();
    }

    [[nodiscard]] int GetRows() const { return m_rows; }
    [[nodiscard]] int GetCols() const { return m_cols; }
    [[nodiscard]] bool IsCardOpen(int row, int col) const { return m_grid[row][col].isOpen; }
    [[nodiscard]] bool IsCardRemoved(int row, int col) const { return m_grid[row][col].isRemoved; }
    [[nodiscard]] int GetTileId(int row, int col) const { return m_grid[row][col].id; }

    void AddListener(IModelListener* listener)
    {
        m_listeners.push_back(listener);
    }

private:
    void NotifyListeners()
    {
        for (auto listener: m_listeners)
        {
            listener->OnModelChanged();
        }
    }
};
