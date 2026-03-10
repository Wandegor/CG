#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "Bezier.h"
#include "BezierRenderer.h"
#include "Window.h"
#include "Shaders/Shader.h"
#include "Common/Point.h"

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

int main()
{
    Bezier bezier;

    Window window(1600, 1000, "Bezier");

    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Шейдеры
    Shader shader(pVSFileName, pFSFileName);
    GLuint shaderProgram = shader.GetProgram();
    // location uniform-переменной
    int colorLocation = glGetUniformLocation(shaderProgram, "uColor");

    BezierRenderer renderer(bezier);

    // Размер точек
    glPointSize(bezier.GetPointSize());
    glEnable(GL_PROGRAM_POINT_SIZE);
    // Сглаживание
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    // DAD
    int selectedPoint = -1;
    bool dragging = false;
    const float pickRadius = 0.04f;

    while (!window.ShouldClose())
    {
        window.ProcessInput();

        double mx, my;
        window.GetNormalizedMousePos(mx, my);
        if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            if (!dragging)
            {
                // Поиск ближайшей контрольной точки
                float bestDist = pickRadius;
                int bestIdx = -1;
                for (int i = 0; i < 4; ++i)
                {
                    float dx = mx - bezier.ControlPoints[i].x;
                    float dy = my - bezier.ControlPoints[i].y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < bestDist)
                    {
                        bestDist = dist;
                        bestIdx = i;
                    }
                }
                if (bestIdx != -1)
                {
                    selectedPoint = bestIdx;
                    dragging = true;
                }
            }
        }
        else
        {
            // Сброс
            dragging = false;
            selectedPoint = -1;
        }

        if (dragging && selectedPoint != -1)
        {
            bezier.ControlPoints[selectedPoint].x = mx;
            bezier.ControlPoints[selectedPoint].y = my;

            auto curvePoints = bezier.GenerateCurvePoints();
            auto dashPoints = bezier.GenerateDashedLines();

            renderer.UpdateCurve(curvePoints);
            renderer.UpdateDashes(dashPoints);
            renderer.UpdatePoints(bezier.ControlPoints, 4);
        }

        glClearColor(1.0f, 0.9f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        renderer.DrawDashes(colorLocation);
        renderer.DrawCurve(colorLocation);
        renderer.DrawPoints(colorLocation);

        glBindVertexArray(0);

        window.SwapBuffers();

        window.PollEvents();
    }

    return 0;
}
