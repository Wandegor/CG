#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "Bezier.h"
#include "Common/Point.h"

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

// Проверка ошибок компиляции шейдера
void CheckShaderCompilation(unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// Проверка линковки программы
void CheckProgramLinking(unsigned int program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
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

    // Настройки OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Cubic Bezier Curve", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1600, 1000);

    auto curvePoints = bezier.GenerateCurvePoints();
    auto dashPoints = bezier.GenerateDashedLines();

    // Позиция
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        }
    )glsl";

    // Цветом
    const char* fragmentShaderSource = R"glsl(
        #version 330 core
        uniform vec4 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = uColor;
        }
    )glsl";

    // Шейдеры
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    CheckShaderCompilation(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    CheckShaderCompilation(fragmentShader, "FRAGMENT");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Получаем location uniform-переменной
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
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

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
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &curveVAO);
    glDeleteBuffers(1, &curveVBO);
    glDeleteVertexArrays(1, &pointsVAO);
    glDeleteBuffers(1, &pointsVBO);
    glDeleteVertexArrays(1, &dashVAO);
    glDeleteBuffers(1, &dashVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
