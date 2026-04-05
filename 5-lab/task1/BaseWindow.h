#pragma once
#include "pch.h"

class BaseWindow
{
public:
    BaseWindow(int w, int h, const char* title)
    {
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        m_window = glfwCreateWindow(w, h, title, nullptr, nullptr);

        if (!m_window)
        {
            throw std::runtime_error("Failed to create window");
        }

        glfwSetWindowUserPointer(m_window, this);

        glfwSetKeyCallback(m_window,
                           [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                               GetBaseWindow(window)->OnKey(key, scancode, action, mods);
                           });
        glfwSetWindowSizeCallback(m_window,
                                  [](GLFWwindow* window, int width, int height)
                                  {
                                      GetBaseWindow(window)->OnResize(width, height);
                                  });
        glfwSetMouseButtonCallback(m_window,
                                   [](GLFWwindow* window, int button, int action, int mods)
                                   {
                                       GetBaseWindow(window)->OnMouseButton(button, action, mods);
                                   });

        glfwSetCursorPosCallback(m_window,
                                 [](GLFWwindow* window, double x, double y)
                                 {
                                     GetBaseWindow(window)->OnMouseMove(x, y);
                                 });
    }

    BaseWindow(const BaseWindow&) = delete;
    BaseWindow& operator=(const BaseWindow&) = delete;

    virtual ~BaseWindow()
    {
        glfwDestroyWindow(m_window);
    }

    [[nodiscard]] glm::ivec2 GetFramebufferSize() const
    {
        int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
        return {w, h};
    }

    void Run()
    {
        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)
        {
            auto* self = static_cast<BaseWindow *>(glfwGetWindowUserPointer(window));
            self->OnMouseMove(x, y);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            auto* self = static_cast<BaseWindow *>(glfwGetWindowUserPointer(window));
            self->OnMouseButton(button, action, mods);
        });

        OnRunStart(); {
            auto size = GetFramebufferSize();
            OnResize(size.x, size.y);
        }

        while (!glfwWindowShouldClose(m_window))
        {
            auto size = GetFramebufferSize();
            Draw(size.x, size.y);
            glFinish();
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        OnRunEnd();
    }

    [[nodiscard]] glm::dvec2 GetCursorPos() const
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        return {x, y};
    }

protected:
    [[nodiscard]] GLFWwindow* GetWindow() const { return m_window; }

private:
    static BaseWindow* GetBaseWindow(GLFWwindow* window)
    {
        return static_cast<BaseWindow *>(glfwGetWindowUserPointer(window));
    }

    virtual void OnKey(int key, int scancode, int action, int mods) {}
    virtual void OnResize([[maybe_unused]] int width, [[maybe_unused]] int height) {}
    virtual void OnMouseButton([[maybe_unused]] int button, [[maybe_unused]] int action, [[maybe_unused]] int mods) {}
    virtual void OnMouseMove([[maybe_unused]] double x, [[maybe_unused]] double y) {}
    virtual void Draw(int width, int height) = 0;
    virtual void OnRunStart() {}
    virtual void OnRunEnd() {}

    static GLFWwindow* MakeWindow(int w, int h, const char* title)
    {
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        return glfwCreateWindow(w, h, title, nullptr, nullptr);
    }

    GLFWwindow* m_window;
};