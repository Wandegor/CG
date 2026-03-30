#include "pch.h"
#include "Window.h"

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr double Z_NEAR = 0.1;
    constexpr double Z_FAR = 50;

    // Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
    glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
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

Window::Window(int w, int h, const char* title, Presenter& presenter)
    : BaseWindow(w, h, title), m_presenter(presenter),
      m_lastTime(glfwGetTime()) {}

void Window::BuildMazeDisplayList(const MazeModel& model)
{
    if (m_wallDisplayList != 0)
    {
        glDeleteLists(m_wallDisplayList, 1);
    }

    m_wallDisplayList = glGenLists(1);
    glNewList(m_wallDisplayList, GL_COMPILE);

    glBegin(GL_QUADS);
    for (int x = 0; x < model.GetWidth(); ++x)
    {
        for (int z = 0; z < model.GetHeight(); ++z)
        {
            if (!model.IsWall(x, z)) continue;

            // Приводим индексы к float для координат
            auto fx = static_cast<float>(x);
            auto fz = static_cast<float>(z);

            // 1. ПЕРЕДНЯЯ ГРАНЬ (Z-)
            if (!model.IsWall(x, z - 1)) {
                glNormal3f(0, 0, -1);
                glVertex3f(fx,     0, fz);
                glVertex3f(fx,     1, fz);
                glVertex3f(fx + 1, 1, fz);
                glVertex3f(fx + 1, 0, fz);
            }

            // 2. ЗАДНЯЯ ГРАНЬ (Z+)
            if (!model.IsWall(x, z + 1)) {
                glNormal3f(0, 0, 1);
                glVertex3f(fx + 1, 0, fz + 1);
                glVertex3f(fx + 1, 1, fz + 1);
                glVertex3f(fx,     1, fz + 1);
                glVertex3f(fx,     0, fz + 1);
            }

            // 3. ЛЕВАЯ ГРАНЬ (X-)
            if (!model.IsWall(x - 1, z)) {
                glNormal3f(-1, 0, 0);
                glVertex3f(fx, 0, fz + 1);
                glVertex3f(fx, 1, fz + 1);
                glVertex3f(fx, 1, fz);
                glVertex3f(fx, 0, fz);
            }

            // 4. ПРАВАЯ ГРАНЬ (X+)
            if (!model.IsWall(x + 1, z)) {
                glNormal3f(1, 0, 0);
                glVertex3f(fx + 1, 0, fz);
                glVertex3f(fx + 1, 1, fz);
                glVertex3f(fx + 1, 1, fz + 1);
                glVertex3f(fx + 1, 0, fz + 1);
            }
        }
    }
    glEnd();
    glEndList();
}

void Window::RenderMaze(const MazeModel& model)
{
    if (m_wallDisplayList == 0)
    {
        BuildMazeDisplayList(model);
    }

    glColor3f(0.6f, 0.2f, 0.6f);
    glCallList(m_wallDisplayList);
}

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

    // Расчет света для обоих сторон полигона
    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

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

    m_presenter.UpdateMovement(deltaTime);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetupCameraMatrix();

    const GLfloat lightPosition[] = {2.0f, 2.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

    RenderMaze(m_presenter.GetModel());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Window::SetupCameraMatrix()
{
    glMatrixMode(GL_MODELVIEW);

    glm::dvec3 pos = m_presenter.GetCameraPos();
    glm::dvec3 front = m_presenter.GetCameraFront();
    glm::dvec3 up = m_presenter.GetCameraUp();

    glm::dmat4 view = glm::lookAt(pos, pos + front, up);
    glLoadMatrixd(&view[0][0]);
}
