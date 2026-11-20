#pragma once
#include "IModelListener.h"
#include "Model.h"
#include "pch.h"

class IView;

class Presenter : public IModelListener
{
private:
    Model m_model;
    IView& m_view;

    bool m_keys[GLFW_KEY_LAST + 1] = {false};
    glm::dvec2 m_mousePos = {0.0, 0.0};

    float m_waitTimer = 0.0f;
    bool m_isAnimation = false;
    int m_row1, m_col1, m_row2, m_col2;

public:
    Presenter(int rows, int cols, IView& view)
        : m_model(rows, cols), m_view(view),
        m_row1(-1), m_col1(-1),
        m_row2(-1), m_col2(-1)
    {
        m_model.AddListener(this);
    }

    void OnKey(int key, int action)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
        {
            if (action == GLFW_PRESS) m_keys[key] = true;
            else if (action == GLFW_RELEASE) m_keys[key] = false;
        }
    }

    void OnWorldClick(double x, double z, float tileSize, float spacing)
    {
        if (m_isAnimation) return;

        int rows = m_model.GetRows();
        int cols = m_model.GetCols();

        // Границы поля
        float totalWidth = cols * tileSize + (cols - 1) * spacing;
        float totalDepth = rows * tileSize + (rows - 1) * spacing;
        float startX = -totalWidth / 2.0f;
        float startZ = -totalDepth / 2.0f;

        // (x,y,z) -> [col][row]
        int col = static_cast<int>((x - startX) / (tileSize + spacing));
        int row = static_cast<int>((z - startZ) / (tileSize + spacing));

        // Проверка на границы матрицы
        if (row >= 0 && row < rows && col >= 0 && col < cols)
        {
            // Попали в зазор (spacing)
            float localX = fmod(x - startX, tileSize + spacing);
            float localZ = fmod(z - startZ, tileSize + spacing);

            if (localX <= tileSize && localZ <= tileSize)
            {
                if (m_model.IsCardRemoved(row, col)) return;
                if (m_model.IsCardOpen(row, col))
                {
                    m_model.SetCardOpen(row, col, false);
                    m_row1 = -1;
                    m_col1 = -1;
                    return;
                }

                if (m_row1 == -1)
                {
                    // Первая карта в паре - открыть
                    m_row1 = row;
                    m_col1 = col;
                    m_model.SetCardOpen(row, col, true);
                }
                else
                {
                    // Вторая - открыть и запустить таймер
                    m_model.SetCardOpen(row, col, true);

                    m_row2 = row;
                    m_col2 = col;

                    // Таймер
                    m_isAnimation = true;
                    m_waitTimer = 0.6f;
                }
                // m_model.PressTile(row, col);
            }
        }
    }

    void Update(float dt)
    {
        if (m_isAnimation)
        {
            m_waitTimer -= dt;
            // Время вышло
            if (m_waitTimer <= 0)
            {
                if (m_model.GetTileId(m_row1, m_col1) ==
                    m_model.GetTileId(m_row2, m_col2))
                {
                    // Совпали
                    m_model.SetCardRemoved(m_row1, m_col1);
                    m_model.SetCardRemoved(m_row2, m_col2);
                }
                else
                {
                    // НЕ совпали
                    m_model.SetCardOpen(m_row1, m_col1, false);
                    m_model.SetCardOpen(m_row2, m_col2, false);

                }

                m_isAnimation = false;
                m_row1 = -1; m_col1 = -1;
                m_row2 = -1; m_col2 = -1;

                m_view.Redraw();
            }
        }
    }

    void OnMouseMove(double x, double y) {}

    [[nodiscard]] const Model& GetModel() const { return m_model; }

    void OnModelChanged() override
    {
        m_view.Redraw();
    }
};
