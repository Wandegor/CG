#include "pch.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ShapeFactory.h"

namespace
{
    // Угол обзора по вертикали
    constexpr float FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr float Z_NEAR = 0.02f;
    constexpr float Z_FAR = 50.0f;

    const char* pVSFileName = "shader.vs";
    const char* pFSFileName = "shader.fs";
} // namespace

Window::Window(int w, int h, const char* title, Presenter& presenter)
    : BaseWindow(w, h, title), m_presenter(presenter),
      m_lastTime(glfwGetTime()) {}

void Window::OnKey(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
    }

    m_presenter.OnKey(key, action);
}

void Window::OnMouseButton(int button, int action, int mods)
{
    m_presenter.OnMouseButton(button, action);
}

void Window::OnMouseMove(double x, double y)
{
    m_presenter.OnMouseMove(x, y);
}

void Window::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::OnRunStart()
{
    m_shader = std::make_unique<Shader>(pVSFileName, pFSFileName);

    auto sphereData = ShapeFactory::CreateMesh(20);

    m_sphere = std::make_unique<RenderObject>(sphereData.vertices, sphereData.indices);

    glEnable(GL_DEPTH_TEST);
}

void Window::SetupLighting() {}

GLuint Window::LoadTexture(const char* path) {}

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    m_presenter.UpdateMovement(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader->GetProgram());

    float time = static_cast<float>(glfwGetTime());
    float morphValue = (sin(time) + 1.0f) / 2.0f;

    GLint morphLoc = glGetUniformLocation(m_shader->GetProgram(), "uMorphTime");
    glUniform1f(morphLoc, morphValue);

    float aspect = static_cast<float>(width) / static_cast<float>(height);

    SetupCameraMatrix(aspect);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_sphere->Draw(GL_TRIANGLES);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::SetupCameraMatrix(float aspect)
{
    glm::mat4 projection = glm::perspective(
        FIELD_OF_VIEW,
        aspect,
        Z_NEAR,
        Z_FAR);

    glm::vec3 pos = m_presenter.GetCameraPos();
    glm::vec3 front = m_presenter.GetCameraFront();
    glm::vec3 up = m_presenter.GetCameraUp();

    glm::mat4 view = glm::lookAt(pos, pos + front, up);

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgram(), "projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
}
