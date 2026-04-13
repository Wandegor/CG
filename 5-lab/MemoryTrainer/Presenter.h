#pragma once
#include "pch.h"
#include "Model.h"

class Presenter
{
private:
    Model m_model;

    bool m_keys[GLFW_KEY_LAST + 1] = {false};
    bool m_leftButtonPressed = false;
    glm::dvec2 m_mousePos = {0.0, 0.0};

    const double m_moveSpeed = 2.0;
    const double m_mouseSensitivity = 0.002;

public:
    Presenter(int width, int height)
        : m_model(width, height)
    {
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
    }

    [[nodiscard]] const Model& GetModel() const { return m_model; }
};
