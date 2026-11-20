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

    void SetCardOpen(int r, int c, bool open)
    {
        m_grid[r][c].isOpen = open;
        NotifyListeners();
    }

    void SetCardRemoved(int r, int c)
    {
        m_grid[r][c].isRemoved = true;
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
