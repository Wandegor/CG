#pragma once
#include <GLFW/glfw3.h>
#include <iostream>

class Window
{
private:
    GLFWwindow* m_window;
    int m_width, m_height;
    const char* m_title;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Window* newWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (newWindow)
            {
            newWindow->m_width = width;
            newWindow->m_height = height;
        }
    }

public:
    Window(int width, int height, const char* title) : m_width(width), m_height(height), m_title(title)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(-1);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this); // Сохраняем указатель на объект для использования в колбэках
        glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    }

    ~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    bool ShouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void SwapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    void PollEvents()
    {
        glfwPollEvents();
    }

    void ProcessInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);
    }

    bool IsMouseButtonPressed(int button) const
    {
        return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
    }

    void GetMousePos(double& x, double& y) const
    {
        glfwGetCursorPos(m_window, &x, &y);
    }

    void GetNormalizedMousePos(double& nx, double& ny) const
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        nx = (x / m_width) * 2.0 - 1.0;
        ny = 1.0 - (y / m_height) * 2.0;
    }

    [[nodiscard]] int GetWidth() const { return m_width; }
    [[nodiscard]] int GetHeight() const { return m_height; }
};
