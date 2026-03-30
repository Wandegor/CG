#include "pch.h"
#include "Window.h"

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr double DISTANCE_TO_ORIGIN = 3;

    constexpr double Z_NEAR = 0.1;
    constexpr double Z_FAR = 10;

    // Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
    glm::dmat4x4 Orthonormalize(const glm::dmat4x4 &m)
    {
        // Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
        const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{m});
        // Заменяем 3 столбца исходной матрицы
        return {
                glm::dvec4{normalizedMatrix[0], 0.0},
                glm::dvec4{normalizedMatrix[1], 0.0},
                glm::dvec4{normalizedMatrix[2], 0.0},
                m[3]
        };
    }
} // namespace

Window::Window(int w, int h, const char *title)
        : BaseWindow(w, h, title),
          m_strip(100, 15),
          m_cameraPos(0.0, 0., 5.0),
          m_yaw(-M_PI / 2.0),
          m_pitch(0.0),
          m_lastTime(glfwGetTime())
{
    UpdateCameraVectors();
}

void Window::UpdateCameraVectors()
{
    // Направление взгляда (front)
    glm::dvec3 front;
    front.x = cos(m_yaw) * cos(m_pitch);
    front.y = sin(m_pitch);
    front.z = sin(m_yaw) * cos(m_pitch);
    m_front = glm::normalize(front);

    // Правый вектор (перпендикулярен front и мировому up)
    m_right = glm::normalize(glm::cross(m_front, glm::dvec3(0.0, 1.0, 0.0)));
    // Вектор вверх (перпендикулярен front и right)
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Window::UpdateMovement(float deltaTime)
{
    float speed = m_moveSpeed * deltaTime;
    glm::dvec3 move(0.0);

    if (m_keys[GLFW_KEY_W]) move.z += speed; // тк Z смотрит в обратную
    if (m_keys[GLFW_KEY_S]) move.z -= speed;
    if (m_keys[GLFW_KEY_A]) move.x -= speed;
    if (m_keys[GLFW_KEY_D]) move.x += speed;

    glm::dvec3 forwardHor = glm::normalize(glm::dvec3(m_front.x, 0.0, m_front.z));
    glm::dvec3 rightHor  = glm::normalize(glm::dvec3(m_right.x, 0.0, m_right.z));

    // Смещение в мировых координатах (только XZ)
    glm::dvec3 deltaWorld = rightHor * move.x + forwardHor * move.z;
    m_cameraPos += deltaWorld;
}

void Window::OnKey(int key, int scancode, int action, int mods)
{
    if (key >= 0 && key <= GLFW_KEY_LAST)
    {
        if (action == GLFW_PRESS)
            m_keys[key] = true;
        else if (action == GLFW_RELEASE)
            m_keys[key] = false;
    }

    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
    }
}

void Window::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        m_leftButtonPressed = (action & GLFW_PRESS) != 0;
    }
}

void Window::OnMouseMove(double x, double y)
{
    const glm::dvec2 mousePos{x, y};
    if (m_leftButtonPressed)
    {
        const glm::dvec2 delta = mousePos - m_mousePos;

        // Изменяем углы с учётом чувствительности
        m_yaw   += delta.x * m_mouseSensitivity;
        m_pitch -= delta.y * m_mouseSensitivity;

        // Ограничиваем pitch, чтобы не переворачиваться (от -89° до +89°)
        const double maxPitch = M_PI / 2.0 - 0.01;
        if (m_pitch > maxPitch) m_pitch = maxPitch;
        if (m_pitch < -maxPitch) m_pitch = -maxPitch;

        UpdateCameraVectors();
    }
    m_mousePos = mousePos;
}

// Вращаем камеру вокруг начала координат
void Window::RotateCamera(double xAngleRadians, double yAngleRadians)
{
}

void Window::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);

    // Вычисляем соотношение сторон клиентской области окна
    double aspect = double(width) / double(height);

    glMatrixMode(GL_PROJECTION);
    const auto proj = glm::perspective(FIELD_OF_VIEW, aspect, Z_NEAR, Z_FAR);
    glLoadMatrixd(&proj[0][0]);
    glMatrixMode(GL_MODELVIEW);
}

void Window::OnRunStart()
{
    // Включаем режим отбраковки граней
    // glEnable(GL_CULL_FACE);
    // Отбраковываться будут нелицевые стороны граней
    // glCullFace(GL_BACK);
    // Сторона примитива считается лицевой, если при ее рисовании
    // обход верших осуществляется против часовой стрелки
    glFrontFace(GL_CCW);

    // Включаем тест глубины для удаления невидимых линий и поверхностей
    glEnable(GL_DEPTH_TEST);

    SetupLighting();
}

void Window::SetupLighting()
{
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    const GLfloat globalAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    const GLfloat lightAmbient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    const GLfloat lightDiffuse[] = {0.60f, 0.60f, 0.60f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void Window::Draw(int width, int height)
{
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - m_lastTime);
    m_lastTime = currentTime;

    UpdateMovement(deltaTime);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCameraMatrix();

    const GLfloat lightPosition[] = {2.0f, 2.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_strip.Draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::SetupCameraMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glm::dmat4 view = glm::lookAt(m_cameraPos, m_cameraPos + m_front, m_up);
    glLoadMatrixd(&view[0][0]);
}
