#include "pch.h"
#include "Window.h"

namespace
{
    // Угол обзора по вертикали
    constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;

    constexpr double STAR_SIZE = 0.35;

    constexpr double Z_NEAR = 0.1;
    constexpr double Z_FAR = 10;

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

Window::Window(int w, int h, const char* title)
    : BaseWindow(w, h, title)
      , m_star(STAR_SIZE)
{
    m_cameraMatrix = glm::lookAt(
        glm::dvec3{ 0.0, 0.0, DISTANCE_TO_ORIGIN },
        glm::dvec3{ 0.0, 0.0, 0.0 },
        glm::dvec3{ 0.0, 1.0, 0.0 });

    GLubyte altha = 120;
    m_star.SetSideColor(0, 255, 0, 0, altha);     // Красный
    m_star.SetSideColor(1, 0, 255, 0, altha);     // Зеленый
    m_star.SetSideColor(2, 0, 0, 255, altha);     // Синий
    m_star.SetSideColor(3, 255, 255, 0, altha);   // Желтый
    m_star.SetSideColor(4, 255, 120, 100, altha); // Коралловый
    m_star.SetSideColor(5, 0, 255, 255, altha);   // Циан
    m_star.SetSideColor(6, 255, 128, 0, altha);   // Оранжевый
    m_star.SetSideColor(7, 128, 0, 123, altha);   // Фиолетовый
    m_star.SetSideColor(8, 200, 128, 128, altha);   // Морская волна
    m_star.SetSideColor(9, 128, 128, 64, altha);   // Оливковый
    m_star.SetSideColor(10, 20, 192, 203, altha); // Ярко-розовый
    m_star.SetSideColor(11, 128, 128, 128, altha);// Серый
    m_star.SetSideColor(12, 139, 69, 19, altha);   // Коричневый (Седло)
    m_star.SetSideColor(13, 255, 20, 147, altha);  // Глубокий розовый
    m_star.SetSideColor(14, 0, 0, 128, altha);     // Темно-синий (Navy)
    m_star.SetSideColor(15, 173, 255, 47, altha);  // Зелено-желтый (Лайм)
    m_star.SetSideColor(16, 218, 112, 214, altha); // Орхидея
    m_star.SetSideColor(17, 230, 209, 204, altha);  // Средний бирюзовый
    m_star.SetSideColor(18, 255, 215, 0, altha);   // Золотой
    m_star.SetSideColor(19, 106, 143, 205, altha);  // Грифельно-синий
}

void Window::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        m_leftMouseButtonPressed = (action == GLFW_PRESS);
        // Запоминаем позицию в момент нажатия, чтобы не было прыжка
        glfwGetCursorPos(GetWindow(), &m_lastMousePos.x, &m_lastMousePos.y);
    }
}

void Window::OnMouseMove(double x, double y)
{
    const glm::dvec2 mousePos{ x, y };

    if (m_leftMouseButtonPressed)
    {
        const auto windowSize = GetFramebufferSize();

        const auto mouseDelta = mousePos - m_lastMousePos;
        const double xAngle = mouseDelta.y * M_PI / windowSize.y;
        const double yAngle = mouseDelta.x * M_PI / windowSize.x;
        RotateCamera(xAngle, yAngle);
    }
    m_lastMousePos = mousePos;
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
    // Включаем тест глубины для удаления невидимых линий и поверхностей
    glEnable(GL_DEPTH_TEST);

    SetupLighting();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::SetupLighting()
{
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    const GLfloat globalAmbient[] = {0.20f, 0.20f, 0.20f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    const GLfloat lightAmbient[]  = {0.20f, 0.20f, 0.20f, 1.0f};
    const GLfloat lightDiffuse[]  = {0.95f, 0.95f, 0.95f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void Window::Draw(int width, int height)
{
    glClearColor(0.67f, 0.67f, 0.67f, 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(&m_cameraMatrix[0][0]);

    const GLfloat lightPosition[] = {2.5f, 2.0f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glDepthMask(GL_TRUE);
    m_star.Draw();
}
