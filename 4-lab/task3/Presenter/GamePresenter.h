#pragma once
#include "../pch.h"
#include "../Model/MazeModel.h"
#include "../View/Window.h"

class GamePresenter
{
public:
    GamePresenter(int w, int h, const char* title)
        : m_view(w, h, title), m_model()
    {
        m_view.onUpdate = [this](double deltaTime) {
            UpdateGameLogic(deltaTime);
        };

        m_view.onDrawMaze = [this]() {
            RenderMaze();
        };

        m_view.onMouseLook = [this](double dx, double dy) {
            double mouseSensitivity = 0.003;
            // Мы передаем только dx, так как по заданию вращение только вокруг вертикальной оси Y
            m_model.Rotate(dx * mouseSensitivity);
        };
    }

    void Run()
    {
        m_view.SetCamera(m_model.GetPosition(), m_model.GetYaw());
        m_view.Run(); // Запускаем бесконечный цикл окна
    }

private:
    void UpdateGameLogic([[maybe_unused]] double deltaTime)
    {
        double speed = 3.0 * deltaTime; // Скорость перемещения
        double forwardAmount = 0.0;
        double rightAmount = 0.0;

        // Читаем нажатия кнопок из View
        if (m_view.IsKeyPressed(GLFW_KEY_W)) forwardAmount += speed;
        if (m_view.IsKeyPressed(GLFW_KEY_S)) forwardAmount -= speed;
        if (m_view.IsKeyPressed(GLFW_KEY_A)) rightAmount -= speed;
        if (m_view.IsKeyPressed(GLFW_KEY_D)) rightAmount += speed;

        // Если есть движение, обновляем Модель
        if (forwardAmount != 0.0 || rightAmount != 0.0)
        {
            m_model.Move(forwardAmount, rightAmount);
        }

        // Синхронизируем: передаем новые координаты из Модели во View
        m_view.SetCamera(m_model.GetPosition(), m_model.GetYaw());
    }

    void RenderMaze()
    {
        // Презентер читает Модель и командует View, что именно рисовать
        for (int x = 0; x < MazeModel::MAZE_SIZE; ++x)
        {
            for (int z = 0; z < MazeModel::MAZE_SIZE; ++z)
            {
                const MazeCell& cell = m_model.GetCell(x, z);
                if (cell.isWall)
                {
                    m_view.DrawWall(x, z, cell.color);
                }
            }
        }
    }

    MazeModel m_model;
    Window m_view;
};