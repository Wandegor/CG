#include "pch.h"
#include "Window.h"

#include "MoebiusStrip.h"
#include "Surface.h"

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;
    // Размер стороны куба
    // constexpr double CUBE_SIZE = 1;

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
          m_cameraMatrix(glm::lookAt(
                  glm::dvec3{0.0, 0.0, DISTANCE_TO_ORIGIN},
                  glm::dvec3{0.0, 0.0, 0.0},
                  glm::dvec3{0.0, 1.0, 0.0})),
          m_lastTime(glfwGetTime())
{}

bool Window::IsKeyPressed(int key) const
{
    return glfwGetKey(GetWindow(), key) == GLFW_PRESS;
}

void Window::UpdateMovement(float deltaTime)
{
    glm::dvec3 move(0.0);
    float speed = m_moveSpeed * deltaTime;

    if (IsKeyPressed(GLFW_KEY_W)) move.z += speed;
    if (IsKeyPressed(GLFW_KEY_S)) move.z -= speed;
    if (IsKeyPressed(GLFW_KEY_A)) move.x -= speed;
    if (IsKeyPressed(GLFW_KEY_D)) move.x += speed;

    if (move.x == 0.0 && move.z == 0.0) return;

    // Получаем локальные оси камеры в мировых координатах
    glm::dvec3 right(m_cameraMatrix[0][0], m_cameraMatrix[1][0], m_cameraMatrix[2][0]);
    // Камера смотрит в направлении -Z, поэтому вектор "вперёд" берём со знаком минус
    glm::dvec3 forward(-m_cameraMatrix[0][2], -m_cameraMatrix[1][2], -m_cameraMatrix[2][2]);

    // Смещение в мировых координатах
    glm::dvec3 deltaWorld = right * move.x + forward * move.z;

    // Применяем трансляцию к матрице вида (умножаем справа)
    m_cameraMatrix = m_cameraMatrix * glm::translate(glm::dmat4(1.0), -deltaWorld);
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
        const auto windowSize = GetFramebufferSize();

        const auto mouseDelta = mousePos - m_mousePos;
        const double xAngle = mouseDelta.y * M_PI / windowSize.y;
        const double yAngle = mouseDelta.x * M_PI / windowSize.x;
        RotateCamera(xAngle, yAngle);
    }
    m_mousePos = mousePos;
}

// Вращаем камеру вокруг начала координат
void Window::RotateCamera(double xAngleRadians, double yAngleRadians)
{
    // Извлекаем из 1 и 2 строки матрицы камеры направления осей вращения,
    // совпадающих с экранными осями X и Y.
    // Строго говоря, для этого надо извлекать столбцы их обратной матрицы камеры, но так как
    // матрица камеры ортонормированная, достаточно транспонировать её подматрицу 3*3
    const glm::dvec3 xAxis{
            m_cameraMatrix[0][0], m_cameraMatrix[1][0], m_cameraMatrix[2][0]
    };
    const glm::dvec3 yAxis{
            m_cameraMatrix[0][1], m_cameraMatrix[1][1], m_cameraMatrix[2][1]
    };
    m_cameraMatrix = glm::rotate(m_cameraMatrix, xAngleRadians, xAxis);
    m_cameraMatrix = glm::rotate(m_cameraMatrix, yAngleRadians, yAxis);

    m_cameraMatrix = Orthonormalize(m_cameraMatrix);
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
    glLoadMatrixd(&m_cameraMatrix[0][0]);
}
