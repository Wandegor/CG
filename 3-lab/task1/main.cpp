#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <fstream>

#include "Bezier.h"
#include "Common/Point.h"

bool ReadFile(const char* fileName, std::string& outFile)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        outFile += line + '\n';
    }
    file.close();
    return true;
}

static void AddShader(GLuint shaderProgram, const char* pShaderText, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    if (shader == 0)
    {
        fprintf(stderr, "Error creating shader%d\n", shaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Length[1];
    Length[0] = std::strlen(pShaderText);

    glShaderSource(shader, 1, p, Length);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        fprintf(stderr, "Error compiling shader type %d:\n%s\n", shaderType, infoLog);
        exit(1);
    }

    glAttachShader(shaderProgram, shader);
    glDeleteShader(shader);
}

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
    if (success == 0)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program\n" << infoLog << std::endl;
        exit(1);
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error validating shader program\n" << infoLog << std::endl;
        exit(1);
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

    // Шейдеры
    std::string vs, fs;

    unsigned int shaderProgram = glCreateProgram();

    if (!ReadFile("shader.vs", vs)) exit(1);
    AddShader(shaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile("shader.fs", fs)) exit(1);
    AddShader(shaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    glLinkProgram(shaderProgram);
    CheckProgramLinking(shaderProgram);

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
