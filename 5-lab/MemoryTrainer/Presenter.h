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
    bool m_leftButtonPressed = false;
    glm::dvec2 m_mousePos = {0.0, 0.0};

    const double m_moveSpeed = 2.0;
    const double m_mouseSensitivity = 0.002;

public:
    Presenter(int rows, int cols, IView& view)
        : m_model(rows, cols), m_view(view)
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
                m_model.PressTile(row, col);
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
