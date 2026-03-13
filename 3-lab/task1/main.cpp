#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "Bezier.h"
#include "BezierRenderer.h"
#include "Window.h"
#include "Shaders/Shader.h"
#include "DragManager.h"
#include "Common/Point.h"

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

int main()
{
    Bezier bezier;

    Window window(1600, 1000, "Bezier");

    if (!gladLoadGL(glfwGetProcAddress))
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
    DragManager dragManager(bezier, renderer, 0.04f);

    while (!window.ShouldClose())
    {
        window.ProcessInput();

        double mx, my;
        window.GetNormalizedMousePos(mx, my);
        bool leftPressed = window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        dragManager.Update(static_cast<float>(mx), static_cast<float>(my), leftPressed);

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
