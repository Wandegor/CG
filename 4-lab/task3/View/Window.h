#pragma once
#include "../pch.h"
#include "BaseWindow.h"
#include "../Model/MazeModel.h"

class Window : public BaseWindow
{
private:
    void UpdatePhysics(double deltaTime)
    {
        double speed = 3.0 * deltaTime;
        double forward = 0, right = 0;

        if (glfwGetKey(GetWindow(), GLFW_KEY_W) == GLFW_PRESS) forward += speed;
        if (glfwGetKey(GetWindow(), GLFW_KEY_S) == GLFW_PRESS) forward -= speed;
        if (glfwGetKey(GetWindow(), GLFW_KEY_A) == GLFW_PRESS) right -= speed;
        if (glfwGetKey(GetWindow(), GLFW_KEY_D) == GLFW_PRESS) right += speed;

        if (forward != 0 || right != 0) m_model.Move(forward, right);
    }

    void RenderMaze()
    {
        for (int x = 0; x < 16; ++x) {
            for (int z = 0; z < 16; ++z) {
                auto& cell = m_model.GetCell(x, z);
                if (cell.isWall) DrawWall(x, z, cell.color);
            }
        }
    }

    void DrawWall(int x, int z, const glm::u8vec4& color)
    {
        glColor3ub(color.r, color.g, color.b);
        double x0 = x, x1 = x + 1.0;
        double z_pos = z; // Плоская стена в координате z
        double h = 1.0;

        glBegin(GL_QUADS);

        glNormal3d(0, 0, 1);

        glVertex3d(x0, 0, z_pos); // 1. Лево-низ
        glVertex3d(x1, 0, z_pos); // 2. Право-низ
        glVertex3d(x1, h, z_pos); // 3. Право-верх
        glVertex3d(x0, h, z_pos); // 4. Лево-верх

        glEnd();
    }

    void DrawEnvironment() { }

    MazeModel m_model; // Ссылка на модель
    double m_lastTime = 0, m_lastX = 0, m_lastY = 0;
    bool m_firstMouse = true;

public:
    // Передаем ссылку на модель в конструктор
    Window(int w, int h, const char* title)
        : BaseWindow(w, h, title) {}

protected:
    void OnRunStart() override
    {
        glEnable(GL_DEPTH_TEST);
        // glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_lastTime = glfwGetTime();
    }

    void OnMouseMove(double x, double y) override
    {
        if (m_firstMouse)
        {
            m_lastX = x; m_lastY = y;
            m_firstMouse = false;
        }
        double dx = x - m_lastX;
        m_lastX = x;

        // Прямо здесь дергаем модель
        m_model.Rotate(dx * 0.003);
    }

    void Draw(int width, int height) override
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;

        UpdatePhysics(deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glm::dvec3 pos = m_model.GetPosition();
        double yaw = m_model.GetYaw();

        glRotated(glm::degrees(-yaw), 0, 1, 0);
        glTranslated(-pos.x, -pos.y, -pos.z);

        DrawEnvironment();

        RenderMaze();
    }
};