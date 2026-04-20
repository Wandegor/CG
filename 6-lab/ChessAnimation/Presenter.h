#pragma once
#include "IModelListener.h"
#include "Model/Model.h"
#include "pch.h"
#include <iostream>

class IView;

class Presenter : public IModelListener
{
private:
    Model m_model;
    IView& m_view;

    bool m_keys[GLFW_KEY_LAST + 1] = {false};
    glm::dvec2 m_mousePos = {0.0, 0.0};

    glm::vec3 m_cameraPos = glm::vec3(0.0f, 5.0f, 7.0f);
    glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    bool m_isLeftMousePressed = false;
    bool m_firstMouse = true; // защита от рывка камеры

    double m_lastX = 0.0;
    double m_lastY = 0.0;

    float m_yaw = -90.0f;
    float m_pitch = -30.0f;

    float m_moveSpeed = 5.0f;

    float m_waitTimer = 0.0f;
    bool m_isAnimation = false;
    Move m_curMove;

    // Позиции в момент анимации
    float m_animX = 0.0f;
    float m_animZ = 0.0f;
    float m_animY = 0.0f;

    float jumpMaxH = 1.0f;



public:
    Presenter(IView& view)
        : m_view(view)
    {
        m_model.AddListener(this);

        UpdateCameraVectors();
    }

    void OnKey(int key, int action)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
        {
            if (action == GLFW_PRESS) m_keys[key] = true;
            else if (action == GLFW_RELEASE) m_keys[key] = false;
        }
    }

    void OnMousePressed(bool pressed)
    {
        m_isLeftMousePressed = pressed;
        if (pressed)
        {
            m_firstMouse = true;
        }
    }

    void OnMouseMove(double x, double y)
    {
        if (!m_isLeftMousePressed) return;

        // 1 кадр после зажатия мыши
        if (m_firstMouse)
        {
            m_lastX = x;
            m_lastY = y;
            m_firstMouse = false;
            return;
        }

        float xOffset = x - m_lastX;
        float yOffset = m_lastY - y;

        m_lastX = x;
        m_lastY = y;

        float sensitivity = 0.1f;
        m_yaw += xOffset * sensitivity;
        m_pitch += yOffset * sensitivity;

        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        UpdateCameraVectors();
    }

    void Update(float dt)
    {
        // wasd движение
        UpdateMovement(dt);

        // ничего не анимируется - следующий ход
        if (!m_isAnimation)
        {
            m_curMove = m_model.GetNextMove();
            if (m_curMove.from.row == -1 || m_curMove.from.col == -1) return;
            m_isAnimation = true;
            m_waitTimer = 0.0f;
        }

        constexpr float duration = 1.5f;
        // Сама анимация
        m_waitTimer += dt;
        float t = m_waitTimer / duration;
        if (t > 1.0f) t = 1.0f;

        // Position = Start + (End - Start) * t
        m_animZ = m_curMove.from.row + (m_curMove.to.row - m_curMove.from.row) * t;
        m_animX = m_curMove.from.col + (m_curMove.to.col - m_curMove.from.col) * t;

        m_animY = jumpMaxH * std::sin(t * M_PI);

        // Время анимации вышло
        if (m_waitTimer >= 1.5f)
        {
            // Изменить модель
            m_model.ApplyMove(m_curMove);
            m_isAnimation = false;
        }

        m_view.Redraw();
    }

    [[nodiscard]] const Model& GetModel() const { return m_model; }

    [[nodiscard]] float GetAnimX() const { return m_animX; }
    [[nodiscard]] float GetAnimZ() const { return m_animZ; }
    [[nodiscard]] float GetAnimY() const { return m_animY; }

    [[nodiscard]] glm::vec3 GetCameraPos() const { return m_cameraPos; }
    [[nodiscard]] glm::vec3 GetCameraFront() const { return m_cameraFront; }
    [[nodiscard]] glm::vec3 GetCameraUp() const { return m_cameraUp; }

    [[nodiscard]] bool IsAnimating() const { return m_isAnimation; }
    [[nodiscard]] Move GetCurrentMove() const { return m_curMove; }

    void OnModelChanged() override
    {
        m_view.Redraw();
    }

private:

    void UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_cameraFront = glm::normalize(front);
    }

    void UpdateMovement(double deltaTime)
    {
        float speed = m_moveSpeed * deltaTime;
        if (m_keys[GLFW_KEY_W]) m_cameraPos += m_cameraFront * speed;
        if (m_keys[GLFW_KEY_S]) m_cameraPos -= m_cameraFront * speed;
        if (m_keys[GLFW_KEY_A]) m_cameraPos -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * speed;
        if (m_keys[GLFW_KEY_D]) m_cameraPos += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * speed;
    }
};
