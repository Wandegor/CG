#pragma once
#include "pch.h"
#include "MazeModel.h"

class Presenter
{
private:
    MazeModel m_model;

    glm::dvec3 m_cameraPos;
    double m_yaw;
    double m_pitch;
    glm::dvec3 m_front;
    glm::dvec3 m_right;
    glm::dvec3 m_up;

    bool m_keys[GLFW_KEY_LAST + 1] = {false};
    bool m_leftButtonPressed = false;
    glm::dvec2 m_mousePos = {0.0, 0.0};

    const double m_moveSpeed = 2.0;
    const double m_mouseSensitivity = 0.002;

    void UpdateCameraVectors()
    {
        glm::dvec3 front;
        front.x = cos(m_yaw) * cos(m_pitch);
        front.y = sin(m_pitch);
        front.z = sin(m_yaw) * cos(m_pitch);
        m_front = glm::normalize(front);

        m_right = glm::normalize(glm::cross(m_front, glm::dvec3(0.0, 1.0, 0.0)));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    void CheckColiseum(glm::dvec3 deltaWorld)
    {
        double offset = 0.15;

        // движение по X
        double nextX = m_cameraPos.x + deltaWorld.x;

        if (!m_model.IsWall(int(nextX + offset), int(m_cameraPos.z + offset)) &&
            !m_model.IsWall(int(nextX + offset), int(m_cameraPos.z - offset)) &&
            !m_model.IsWall(int(nextX - offset), int(m_cameraPos.z + offset)) &&
            !m_model.IsWall(int(nextX - offset), int(m_cameraPos.z - offset)))
        {
            m_cameraPos.x = nextX;
        }

        // движение по Z
        double nextZ = m_cameraPos.z + deltaWorld.z;

        // Аналогично: проверяем все 4 угла квадрата в новой позиции по Z
        if (!m_model.IsWall(int(m_cameraPos.x + offset), int(nextZ + offset)) &&
            !m_model.IsWall(int(m_cameraPos.x + offset), int(nextZ - offset)) &&
            !m_model.IsWall(int(m_cameraPos.x - offset), int(nextZ + offset)) &&
            !m_model.IsWall(int(m_cameraPos.x - offset), int(nextZ - offset)))
        {
            m_cameraPos.z = nextZ;
        }
    }

public:
    Presenter(int mazeW, int mazeH)
        : m_model(mazeW, mazeH),
          m_cameraPos(1.5, 0.5, 1.5),
          m_yaw(-M_PI / 2.0),
          m_pitch(0.0)
    {
        UpdateCameraVectors();
    }

    void UpdateMovement(double deltaTime)
    {
        double speed = m_moveSpeed * deltaTime;
        glm::dvec3 move(0.0);

        if (m_keys[GLFW_KEY_W]) move.z += speed;
        if (m_keys[GLFW_KEY_S]) move.z -= speed;
        if (m_keys[GLFW_KEY_A]) move.x -= speed;
        if (m_keys[GLFW_KEY_D]) move.x += speed;

        glm::dvec3 forwardHor = glm::normalize(glm::dvec3(m_front.x, 0.0, m_front.z));
        glm::dvec3 rightHor = glm::normalize(glm::dvec3(m_right.x, 0.0, m_right.z));

        glm::dvec3 deltaWorld = rightHor * move.x + forwardHor * move.z;

        // Коллизии
        CheckColiseum(deltaWorld);
    }

    void OnKey(int key, int action)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
        {
            if (action == GLFW_PRESS) m_keys[key] = true;
            else if (action == GLFW_RELEASE) m_keys[key] = false;
        }
    }

    void OnMouseButton(int button, int action)
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            m_leftButtonPressed = (action & GLFW_PRESS) != 0;
        }
    }

    void OnMouseMove(double x, double y)
    {
        glm::dvec2 currentMousePos{x, y};
        if (m_leftButtonPressed)
        {
            glm::dvec2 delta = currentMousePos - m_mousePos;
            m_yaw += delta.x * m_mouseSensitivity;
            m_pitch -= delta.y * m_mouseSensitivity;

            const double maxPitch = M_PI / 2.0 - 0.01;
            if (m_pitch > maxPitch) m_pitch = maxPitch;
            if (m_pitch < -maxPitch) m_pitch = -maxPitch;

            UpdateCameraVectors();
        }
        m_mousePos = currentMousePos;
    }

    [[nodiscard]] const MazeModel& GetModel() const { return m_model; }
    [[nodiscard]] glm::dvec3 GetCameraPos() const { return m_cameraPos; }
    [[nodiscard]] glm::dvec3 GetCameraFront() const { return m_front; }
    [[nodiscard]] glm::dvec3 GetCameraUp() const { return m_up; }
};
