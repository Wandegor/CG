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

    int count = 3;
    float h[] = { 0.0f, 0.8f, 1.5f };
    float r[] = { 1.0f, 0.7f, 0.4f };
    float sH[] = { 0.7f, 0.6f, 0.5f };
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

    auto quadData = ShapeFactory::CreateMesh();

    m_quad = std::make_unique<RenderObject>(quadData.vertices, quadData.indices, 5);

    glDisable(GL_DEPTH_TEST);
}

void Window::SetupLighting()
{
    glUseProgram(m_shader->GetProgram());

    m_shader->SetVec3("lightPos", glm::vec3(2.0f, 6.0f, 5.0f));
    m_shader->SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    m_shader->SetVec3("objectColor", glm::vec3(1.0f, 0.7f, 0.5f));

    // Фоновые свойства материала
    m_shader->SetFloat("ambient", 0.45f); // интенсивность
    m_shader->SetFloat("matAmbient", 1.0f); // Насколько материал "принимает" фон

    // Цвет фонового излучения источника
    m_shader->SetVec3("lightAmbient", glm::vec3(0.2f, 0.2f, 0.2f));

    // Блик
    m_shader->SetVec3("specularColor", glm::vec3(0.2f, 0.8f, 0.1f));
    m_shader->SetFloat("shininess",160.0f);

    m_shader->SetVec3("viewPos", m_presenter.GetCameraPos());
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

    GLuint shaderID = m_shader->GetProgram();
    glUseProgram(shaderID);

    glUniform1i(glGetUniformLocation(shaderID, "numObjects"), count);
    glUniform1fv(glGetUniformLocation(shaderID, "heights"), count, h);
    glUniform1fv(glGetUniformLocation(shaderID, "rads"), count, r);
    glUniform1fv(glGetUniformLocation(shaderID, "sizesH"), count, sH);

    SetupLighting();

    float aspect = static_cast<float>(width) / static_cast<float>(height);

    SetupCameraMatrix(aspect);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_quad->Draw(GL_TRIANGLES);
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
