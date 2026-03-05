#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Функция обратного вызова для изменения размера окна
void Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Обработка нажатия клавиши Escape для выхода
void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // Инициализация GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Настройки OpenGL (используем Core Profile 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);

    // Инициализация GLAD (загрузка функций OpenGL)
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Устанавливаем область вывода (viewport)
    glViewport(0, 0, 800, 600);

    float vertices[] =
    {   // pos                  // colors
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f,
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Шейдеры
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "void main() { gl_Position = vec4(aPos, 1.0); ourColor = aColor; }";

    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 ourColor;\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(ourColor, 1.0); }";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Главный цикл
    while (!glfwWindowShouldClose(window))
    {
        // Обработка ввода
        ProcessInput(window);

        // Очистка экрана тёмно-синим цветом
        glClearColor(0.1f, 0.2f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Здесь будет ваш код рендеринга
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Обмен буферов и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    // Очистка
    glfwTerminate();
    return 0;
}