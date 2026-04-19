#pragma once
#include "IModelListener.h"
#include "Model/Model.h"
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
    Move m_curMove;

    // Позиции в момент анимации
    float m_animX = 0.0f;
    float m_animY = 0.0f;

public:
    Presenter(IView& view)
        : m_view(view)
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

    void OnWorldClick(double x, double z, float tileSize, float spacing) {}

    void Update(float dt)
    {
        // ничего не анимируется - следующий ход
        if (!m_isAnimation)
        {
            m_curMove = m_model.GetNextMove();
            m_isAnimation = true;
            m_waitTimer = 0.0f;
        }

        // Сама анимация
        m_waitTimer += dt;
        float t = m_waitTimer / 2.0f;

        // Position = Start + (End - Start) * t
        m_animX = m_curMove.from.row + (m_curMove.to.row - m_curMove.from.row) * t;
        m_animY = m_curMove.from.col + (m_curMove.to.col - m_curMove.from.col) * t;

        // Время анимации вышло
        if (m_waitTimer >= 1.0f)
        {
            // Изменить модель
            m_model.ApplyMove(m_curMove);

            m_isAnimation = false;
        }

        m_view.Redraw();
    }

    void OnMouseMove(double x, double y) {}

    [[nodiscard]] const Model& GetModel() const { return m_model; }

    float GetAnimX() const { return m_animX; }
    float GetAnimY() const { return m_animY; }
    bool IsAnimating() const { return m_isAnimation; }
    Move GetCurrentMove() const { return m_curMove; }

    void OnModelChanged() override
    {
        m_view.Redraw();
    }
};
