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

    auto sphereData = ShapeFactory::CreateMesh();

    m_sphere = std::make_unique<RenderObject>(sphereData.vertices, sphereData.indices, 8);

    glEnable(GL_DEPTH_TEST);
}

void Window::SetupLighting()
{
    glUseProgram(m_shader->GetProgram());

    glm::vec3 lightPosition = glm::vec3(2.0f, 3.0f, 5.0f);
    m_shader->SetVec3("lightPos", lightPosition);

    glm::vec3 color = glm::vec3(1.0f, 0.7f, 0.5f);
    m_shader->SetVec3("objectColor", color);

    m_shader->SetFloat("ambient", 0.05f);

    m_shader->SetVec3("viewPos", m_presenter.GetCameraPos());

    glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_shader->SetVec3("specularColor", specularColor);

    m_shader->SetFloat("shininess",32.0f);
}

GLuint Window::LoadTexture(const char* path) {}

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    m_presenter.UpdateMovement(deltaTime);

    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader->GetProgram());

    SetupLighting();

    float aspect = static_cast<float>(width) / static_cast<float>(height);

    SetupCameraMatrix(aspect);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
