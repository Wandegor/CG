#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "Bezier.h"
#include "Window.h"
#include "Shaders/Shader.h"
#include "Common/Point.h"

const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

// Выход по Escape
void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Изменение размера окна
void Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    Bezier bezier;
    // Инициализация GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    Window window(1600, 1000, "Bezier");

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1600, 1000);

    auto curvePoints = bezier.GenerateCurvePoints();
    auto dashPoints = bezier.GenerateDashedLines();

    // Шейдеры
    Shader shader(pVSFileName, pFSFileName);
    GLuint shaderProgram = shader.GetProgram();
    // location uniform-переменной
    int colorLocation = glGetUniformLocation(shaderProgram, "uColor");

    // Кривая
    GLuint curveVAO, curveVBO;
    glGenVertexArrays(1, &curveVAO);
    glGenBuffers(1, &curveVBO);
    glBindVertexArray(curveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
    glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(Point), curvePoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Контрольные точки
    GLuint pointsVAO, pointsVBO;
    glGenVertexArrays(1, &pointsVAO);
    glGenBuffers(1, &pointsVBO);
    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bezier.ControlPoints), bezier.ControlPoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Пунктир
    unsigned int dashVAO, dashVBO;
    glGenVertexArrays(1, &dashVAO);
    glGenBuffers(1, &dashVBO);
    glBindVertexArray(dashVAO);
    glBindBuffer(GL_ARRAY_BUFFER, dashVBO);
    glBufferData(GL_ARRAY_BUFFER, dashPoints.size() * sizeof(Point), dashPoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Размер точек
    glPointSize(bezier.GetPointSize());
    glEnable(GL_PROGRAM_POINT_SIZE);
    // Сглаживание
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    // Главный цикл
    while (!window.ShouldClose())
    {
        window.ProcessInput();

        // Очистка экрана
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // пунктирные линии (серые)
        glUniform4f(colorLocation, 0.6f, 0.6f, 0.6f, 1.0f);
        glBindVertexArray(dashVAO);
        glDrawArrays(GL_LINES, 0, dashPoints.size());

        // кривую Безье (синяя)
        glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
        glBindVertexArray(curveVAO);
        glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());

        // контрольные точки
        glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        glBindVertexArray(pointsVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glDrawArrays(GL_POINTS, 1, 1);
        glDrawArrays(GL_POINTS, 2, 1);
        glDrawArrays(GL_POINTS, 3, 1);

        // Отвязываем VAO
        glBindVertexArray(0);

        // Обмен буферов и обработка событий
        window.SwapBuffers();
        window.PollEvents();
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &curveVAO);
    glDeleteBuffers(1, &curveVBO);
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteBuffers(1, &pointsVBO);
    glDeleteVertexArrays(1, &dashVAO);
    glDeleteBuffers(1, &dashVBO);
    glDeleteProgram(shaderProgram);

    return 0;
}
